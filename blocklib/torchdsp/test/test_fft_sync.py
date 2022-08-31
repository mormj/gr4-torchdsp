#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# Author: josh
# GNU Radio version: 4.0.0.0-preview0

from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
from gnuradio import streamops
from gnuradio import torchdsp
from matplotlib import pyplot as plt

from gnuradio.schedulers.nbt import scheduler_nbt as nbt
from gnuradio.schedulers.triton import scheduler_triton as tsched


def snipfcn_snippet_0(fg, rt=None):
    plt.plot(fg.snk.data())
    plt.show()


def snippets_main_after_stop(fg, rt=None):
    snipfcn_snippet_0(fg, rt)


class untitled(gr.flowgraph):

    def __init__(self):
        gr.flowgraph.__init__(self, "Not titled yet")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 32000
        self.nitems = nitems = 512*100

        ##################################################
        # Blocks
        ##################################################
        nblocks = 1
        # Setting the async flag to true will require the async triton scheduler
        # blk = torchdsp.triton_block(2, 1, "add_cpu_openvino", True, 'localhost:8000', [], [])
        
        hd = streamops.head( nitems,0, impl=streamops.head.cpu)
        self.snk = snk = blocks.vector_sink_c( 1,nitems, impl=blocks.vector_sink_c.cpu)
        src1 = analog.sig_source_c( samp_rate,analog.waveform_t.COS,1234,1.0,0,0, impl=analog.sig_source_c.cpu)
        # src2 = analog.sig_source_f( samp_rate,analog.waveform_t.COS,1000,1.0,0,0, impl=analog.sig_source_f.cpu)


        ##################################################
        # Connections
        ##################################################
        blks = []
        for ii in range(nblocks):
            blk = torchdsp.triton_block(1, 1, "fft_cpu_openvino", False, 'localhost:8000', [], [])

            if ii == 0:
                self.connect((src1, 0), (blk, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make())
            else:
                self.connect((last_blk, 0), (blk, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make())

            if ii == nblocks-1:
                self.connect((blk, 0), (hd, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make())
                
            blks.append(blk)
            last_blk = blk
        # self.connect((src2, 0), (blk, 1)).set_custom_buffer(torchdsp.buffer_triton_properties.make())
        self.connect((hd, 0), (snk, 0))
        


        sched1 = nbt()
        sched2 = tsched()

        self.rt = gr.runtime()

        # self.rt.add_scheduler((sched1, [src1, hd, snk]))
        # self.rt.add_scheduler((sched2, blks))





def main(flowgraph_cls=untitled, options=None):
    fg = flowgraph_cls()
    
    rt = fg.rt

    rt.initialize(fg)

    rt.start()

    try:
        rt.wait()
    except KeyboardInterrupt:
        rt.stop()
        rt.wait()
    snippets_main_after_stop(fg, rt)

if __name__ == '__main__':
    main()
