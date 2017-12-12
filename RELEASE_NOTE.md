# OpenAirInterface System Emulation
## Compiling oaisim without S1
```
> cd $OPENAIR_HOME/cmake_targets
> sudo ./build_oai --oaisim --noS1 -c -x -g
```
The oaisim executable (`oaisim_nos1`) is now located under: 
`$OPENAIR_HOME/cmake_targets/oaisim_noS1_build_oai/build/`
## Running oaisim without S1
```
> cd $OPENAIR_HOME
> source oaienv
> cd $OPENAIR_HOME/cmake_targets/oaisim_noS1_build_oai/build
> ./oaisim_nos1 -O $OPENAIR_HOME/targets/PROJECTS/GENERIC-LTE-EPC/CONF/enb.band7.generic.oaisim.local_no_mme.conf -c 41  #no. 41 template
```
For more xml templates, please look at: 
`$OPENAIR_HOME/targets/SIMU/EXAMPLES/OSD/WEBXML/`



## Reference

[Openairlteemulation](https://gitlab.eurecom.fr/oai/openairinterface5g/wikis/OpenAirLTEEmulation)
