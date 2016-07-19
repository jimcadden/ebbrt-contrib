//          Copyright Boston University SESA Group 2013 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "Printer.h"
#include "netpipe.h"

#include <ebbrt/Debug.h>

void AppMain() {

  int argc = 1;
  char **argv = nullptr;

  ebbrt::kprintf("Calling netpipe_main");
  netpipe_main(argc, argv);
  ebbrt::kprintf("netpipe_main exited");
  /* int argc = 1; */
  /* char **argv = nullptr; */
  /* ArgStruct args;            /1* Arguments for all the calls *1/ */
  /* Init(&args, &argc, &argv); /1* This will set args.tr and args.rcv *1/ */
  /*   args.preburst = 0; /1* Default to not bursting preposted receives *1/ */
  /*   args.bidir = 0; /1* Turn bi-directional mode off initially *1/ */
  /*   args.cache = 1; /1* Default to use cache *1/ */
  /*   args.upper = MAXINT; */
  /*   args.host  = NULL; */
  /*   args.soffset=0; /1* default to no offsets *1/ */
  /*   args.roffset=0; */
  /*   args.syncflag=0; /1* use normal mpi_send *1/ */
  /*   args.use_sdp=0; /1* default to no SDP *1/ */
  /*   args.port = DEFPORT; /1* just in case the user doesn't set this. *1/ */
  /*  args.nbuff = TRIALS; */
  /* ebbrt::kprintf("Init completed\n"); */
  /* Setup(&args); */
  /* ebbrt::kprintf("Setup completed\n"); */
}

