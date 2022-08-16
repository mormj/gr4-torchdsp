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
        self.nitems = nitems = 1024

        ##################################################
        # Blocks
        ##################################################
        self.torchdsp_add_0 = torchdsp.triton_block(2, 1, "add_cpu_openvino", 256, 'localhost:8000', [], [])
        self.streamops_head_0 = streamops.head( nitems,0, impl=streamops.head.cpu)
        self.snk = blocks.vector_sink_f( 1,nitems, impl=blocks.vector_sink_f.cpu)
        self.analog_sig_source_0_0 = analog.sig_source_f( samp_rate,analog.waveform_t.COS,1234,1.0,0,0, impl=analog.sig_source_f.cpu)
        self.analog_sig_source_0 = analog.sig_source_f( samp_rate,analog.waveform_t.COS,1000,1.0,0,0, impl=analog.sig_source_f.cpu)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_sig_source_0, 0), (self.torchdsp_add_0, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make())
        self.connect((self.analog_sig_source_0_0, 0), (self.torchdsp_add_0, 1)).set_custom_buffer(torchdsp.buffer_triton_properties.make())
        self.connect((self.streamops_head_0, 0), (self.snk, 0))
        self.connect((self.torchdsp_add_0, 0), (self.streamops_head_0, 0)).set_custom_buffer(torchdsp.buffer_triton_properties.make())


    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_nitems(self):
        return self.nitems

    def set_nitems(self, nitems):
        self.nitems = nitems




def main(flowgraph_cls=untitled, options=None):
    fg = flowgraph_cls()
    rt = gr.runtime()


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
