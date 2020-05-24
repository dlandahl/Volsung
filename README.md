## Overview
### Introduction
http://landahl.tech/volsung/ <br />
Volsung is a new language for audio, currently in development. It can be used for sound synthesis, audio effect processing, and generative music composition. Aimed at musicians, it is designed to be visual and intuitive, while providing language constructs and abstractions that enable readable and terse algorithm design. Volsung draws inspiration from similar DSP languages such as Miller Puckette's [Pure Data](https://puredata.info), Stanford's [ChucK](http://chuck.stanford.edu), and [FAUST](https://faust.grame.fr), developed at the Générateur de Ressources et d’Activités Musicales Exploratoires (GRAME).


### Data-flow programming
Volsung employs the data-flow programming paradigm. Data-flow programming is based around creating directed mathematical graphs, the verteces of which represent procedures, and the edges of which represent data. Each edge holds input parameters of one procedure and return data of another. The graph is constructed by declaring audio or logic processing devices (also objects), and connecting them using arrow operators. The basic connection operator `->` connects an output port on one object to an input port on another. Data-flow systems are reminiscent of things like guitar pedal-boards and modular synthesizers, and are already familiar for most creative pracitioners in the field of audio.


### Front-ends
Deployment through a wide range of existing technologies is planned, including Steinberg VST, VCVRack module, Unreal engine, Audiokinetic Wwise plugin, and Eurorack hardware.
