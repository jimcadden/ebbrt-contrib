//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Printer.h"
#include <boost/config.hpp>
#include <mapreduce.hpp>

namespace prime_calculator {

bool const is_prime(long const number)
{
    if (number > 2)
    {
        if (number % 2 == 0)
            return false;

        long const n = std::abs(number);
        long const sqrt_number = static_cast<long>(std::sqrt(static_cast<double>(n)));

        for (long i = 3; i < sqrt_number; i+=2)
        {
            if (n % i == 0)
                return false;
        }
    }
    else if (number == 0 || number == 1)
        return false;
    
    return true;
}

template<typename MapTask>
class number_source : mapreduce::detail::noncopyable
{
  public:
    number_source(long first, long last, long step)
      : sequence_(0), first_(first), last_(last), step_(step)
    {
    }

    bool const setup_key(typename MapTask::key_type &key)
    {
        key = sequence_++;
        return (key * step_ <= last_);
    }

    bool const get_data(typename MapTask::key_type const &key, typename MapTask::value_type &value)
    {
        typename MapTask::value_type val;

        val.first  = first_ + (key * step_);
        val.second = std::min(val.first + step_ - 1, last_);

        std::swap(val, value);
        return true;
    }

  private:
    long       sequence_;
    long const first_;
    long const last_;
    long const step_;
};

struct map_task : public mapreduce::map_task<long, std::pair<long, long> >
{
    template<typename Runtime>
    void operator()(Runtime &runtime, key_type const &/*key*/, value_type const &value) const
    {
        for (key_type loop=value.first; loop<=value.second; ++loop)
            runtime.emit_intermediate(is_prime(loop), loop);
    }
};

struct reduce_task : public mapreduce::reduce_task<bool, long>
{
    template<typename Runtime, typename It>
    void operator()(Runtime &runtime, key_type const &key, It it, It ite) const
    {
        if (key)
            std::for_each(it, ite, std::bind(&Runtime::emit, &runtime, true, std::placeholders::_1));
    }
};

typedef
mapreduce::job<prime_calculator::map_task,
               prime_calculator::reduce_task,
               mapreduce::null_combiner,
               prime_calculator::number_source<prime_calculator::map_task>
> job;

} // namespace prime_calculator

void AppMain() { printer->Print("MAPREDUCE BACKEND UP.\n"); 

    mapreduce::specification spec;
    int prime_limit = 10000;
    spec.map_tasks = 1;
    int reduce_tasks = 1;
    prime_calculator::job::datasource_type datasource(0, prime_limit, prime_limit/reduce_tasks);
    ebbrt::kprintf("Calculating Prime Numbers in the range of 0 .. %d\n", prime_limit);
    prime_calculator::job job(datasource, spec);
    mapreduce::results result;

    job.run<mapreduce::schedule_policy::sequential<prime_calculator::job> >(result);

    ebbrt::kprintf("MapReduce finished in %d with %d results \n", result.job_runtime.count(), std::distance(job.begin_results(), job.end_results()));
    for (auto it=job.begin_results(); it!=job.end_results(); ++it)
        ebbrt::kprintf("%d ",it->second);
    ebbrt::kprintf("\n");
}
