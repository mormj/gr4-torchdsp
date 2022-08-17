#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# GNU Radio version: 3.9.0.0-git

from gnuradio import blocks, streamops
from gnuradio import torchdsp
from gnuradio import gr
from gnuradio.schedulers import nbt
import sys
import signal
from argparse import ArgumentParser
import time

class benchmark_copy(gr.top_block):

    def __init__(self, args):
        gr.top_block.__init__(self, "Benchmark Copy")
        self.blocks = []
        ##################################################
        # Variables
        ##################################################
        nsamples = args.samples
        veclen = args.veclen
        self.actual_samples = actual_samples = int(nsamples /  veclen)
        num_blocks = args.nblocks

        ##################################################
        # Blocks
        ##################################################
        op_blocks = []
        for i in range(num_blocks):
            op_blocks.append(
                torchdsp.triton_block(2, 1, f'{args.operation}_{args.device}', args.batch_size, args.addr, [], [])
            )
        self.blocks.extend(op_blocks)

        src = blocks.null_source()
        snk = blocks.null_sink()
        hd = streamops.head(actual_samples, 
            gr.sizeof_float*veclen)

        self.blocks.extend([src,snk,hd])
        ##################################################
        # Connections
        ##################################################
        buffer_size = args.buffer_size
        self.connect((hd, 0), (snk, 0))
        for idx, blk in enumerate(op_blocks):
            if (idx == 0):
                self.connect((src, 0), (blk, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make().set_buffer_size(buffer_size))
            else:
                self.connect(op_blocks[idx-1], (blk, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make().set_buffer_size(buffer_size))

            ns = blocks.null_source()
            self.blocks.append(ns)
            self.connect((ns, 0), (blk, 1)).set_custom_buffer(torchdsp.buffer_triton_properties.make().set_buffer_size(buffer_size))


        self.connect((op_blocks[num_blocks-1], 0), (hd, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make().set_buffer_size(buffer_size))


def main(top_block_cls=benchmark_copy, options=None):

    parser = ArgumentParser(description='Run a flowgraph iterating over parameters for benchmarking')
    parser.add_argument('--rt_prio', help='enable realtime scheduling', action='store_true')
    parser.add_argument('--samples', type=int, default=1e8)
    parser.add_argument('--operation', type=str, default='add')
    parser.add_argument('--device', type=str, default='cpu_openvino')
    parser.add_argument('--nblocks', type=int, default=1)
    parser.add_argument('--veclen', type=int, default=1)
    parser.add_argument('--addr', type=str, default='localhost:8000')
    parser.add_argument('--batch-size', type=int, default=256)
    parser.add_argument('--buffer-size', type=int, default=64000)
    parser.add_argument('--group', action='store_true')

    args = parser.parse_args()
    print(args)

    if args.rt_prio and gr.enable_realtime_scheduling() != gr.RT_OK:
        print("Error: failed to enable real-time scheduling.")

    tb = top_block_cls(args)
    rt = gr.runtime()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()
        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    sched = nbt.scheduler_nbt("sched")
    if (args.group):
        sched.add_block_group(tb.blocks)

    rt.add_scheduler(sched)

    rt.initialize(tb)
    print("starting ...")
    startt = time.time()
    rt.start()

    rt.wait()
    endt = time.time()

    print(f'[PROFILE_TIME]{endt-startt}[PROFILE_TIME]')

if __name__ == '__main__':
    main()
