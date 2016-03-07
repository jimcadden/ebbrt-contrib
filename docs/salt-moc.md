# SESA on the MOC

### node configuration and depoyment

---

## Overview

- Default network boot configuration
- Configure and deploy applications

---

## Backgroud
- EbbRT OS for distributed Cloud applications
- Experimentation and benchmarking
    - networking latecy/throughput
    - elastic deployment
    - native vs. virtualized
- HaaS bare-metal deployment

---

## Challenge
- Boot machine with particular configuration 
- Deploy and capture results 
- Identical redeployments

---

## Problem
- Many configurations
- Changes applied across configurations

---

## Early Solution
- Debirf: Diskless all-in-ram images
        - kernel + initfs
        - application + dependancies 
- State is hard-coded and contained
- One image, one configuration

#### Problem?

---

## Current Solution
- Network boot into base system image
- Use SaltStack remote configuration

---

## Network Boot (Headnode) 
- PXE / TFTP 
- Configure linux with **read-only** `nfsroot`
        kernel fs/vmlinuz
        initrd fs/initrd.img
        append root=/dev/nfs nfsroot=$IP:/var/lib/tftpboot/fs ro       
- Union mount `overlayfs`

---

## SaltStack
#### Remote Configuration and Execution
- Salt Master on Headnode
- Nodes pre-configured as Salt Minions

