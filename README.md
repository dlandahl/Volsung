## Overview
### Introduction

Volsung is a new language for audio, currently in development. It can be used for sound synthesis, audio effect processing, and procedural game audio. Aimed at sound designers, it prioritises being visual and intuitive while providing language constructs and abstractions that enable readable and terse algorithm specification. Volsung draws inspiration from similar DSP languages such as Miller Puckette's [Pure Data](https://puredata.info), Stanford's [ChucK](http://chuck.stanford.edu), and [FAUST](https://faust.grame.fr), developed at the Générateur de Ressources et d’Activités Musicales Exploratoires (GRAME).


### Data-flow programming
Volsung employs the data-flow programming paradigm. Data-flow programming is based around creating directed mathematical graphs, the vertices (or nodes) of which represent processes, and the edges (or connections) of which represent data. Each edge holds input data of one process and output data of another. The graph is constructed by declaring audio or logic processing devices (also objects), and connecting them using arrow operators. The basic connection operator `->` connects an output port on one object to an input port on another. 


### Front-ends
Deployment through a wide range of existing technologies is planned, including Steinberg VST, VCVRack module, Unreal engine, Audiokinetic Wwise, and Eurorack hardware.

<br /><br /> Try it now at: http://landahl.tech/volsung/ 
