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
from gnuradio.schedulers.nbt import scheduler_nbt as nbt
from gnuradio.schedulers.triton import scheduler_triton as tsched
import sys
import signal
from argparse import ArgumentParser
import time

class benchmark_copy(gr.top_block):

    def __init__(self, args):
        gr.top_block.__init__(self, "Benchmark Copy")
        self.gr_blocks = []
        self.tr_blocks = []
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
                torchdsp.triton_block(1, 1, f'{args.operation}_{args.device}', args.async_sched, args.addr, [], [])
            )
        self.tr_blocks.extend(op_blocks)

        src = blocks.null_source()
        snk = blocks.null_sink()
        hd = streamops.head(actual_samples, 
            gr.sizeof_float*veclen)

        self.gr_blocks.extend([src,snk,hd])
        ##################################################
        # Connections
        ##################################################
        buffer_size = args.buffer_size
        buffer_size = args.batch_size * gr.sizeof_gr_complex * args.op_size
        self.connect((hd, 0), (snk, 0))
        for idx, blk in enumerate(op_blocks):
            if (idx == 0):
                self.connect((src, 0), (blk, 0))\
                    .set_custom_buffer(torchdsp.buffer_triton_properties.make().set_buffer_size(buffer_size))
            else:
                self.connect((op_blocks[idx-1],0), (blk, 0))\
                    .set_custom_buffer(torchdsp.buffer_triton_properties.make().set_buffer_size(buffer_size))

            if args.operation in ['add']:
                ns = blocks.null_source()
                self.gr_blocks.append(ns)
                self.connect((ns, 0), (blk, 1)).set_custom_buffer(torchdsp.buffer_triton_properties.make()\
                    .set_buffer_size(buffer_size))


        self.connect((op_blocks[num_blocks-1], 0), (hd, 0))\
            .set_custom_buffer(torchdsp.buffer_triton_properties.make().set_buffer_size(buffer_size))


def main(top_block_cls=benchmark_copy, options=None):

    parser = ArgumentParser(description='Run a flowgraph iterating over parameters for benchmarking')
    parser.add_argument('--rt_prio', help='enable realtime scheduling', action='store_true')
    parser.add_argument('--samples', type=int, default=1e8)
    parser.add_argument('--operation', type=str, default='fft')
    parser.add_argument('--device', type=str, default='cpu_openvino')
    parser.add_argument('--nblocks', type=int, default=1)
    parser.add_argument('--veclen', type=int, default=1)
    parser.add_argument('--op_size', type=int, default=512)
    parser.add_argument('--addr', type=str, default='localhost:8000')
    parser.add_argument('--batch_size', type=int, default=16)
    parser.add_argument('--buffer_size', type=int, default=64000)
    parser.add_argument('--group', action='store_true')
    parser.add_argument('--async_sched', action='store_true')

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

    sched1 = nbt()
    sched2 = tsched()

    rt = gr.runtime()

    if args.async_sched:
        rt.add_scheduler((sched1, tb.gr_blocks))
        rt.add_scheduler((sched2, tb.tr_blocks))
    else:
        if (args.group):
            sched1.add_block_group(tb.gr_blocks + tb.tr_blocks)
        rt.add_scheduler(sched1)

    
    rt.initialize(tb)
    print("starting ...")
    startt = time.time()
    rt.start()

    rt.wait()
    endt = time.time()

    print(f'[PROFILE_TIME]{endt-startt}[PROFILE_TIME]')

if __name__ == '__main__':
    main()
