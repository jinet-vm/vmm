# Jinet VMM

A VMM (*Virtual Machine Monitor*) of the Jinet hypervisor.

## Config file

All neccessary numeric consts used across `asm`, `c` source files and `ld`
scripts should be defined in [config.json](config.json). Here's an example of how some value can be stored:

```json
{
    "const1": "value",
    "const2": "value"
}
```

*Warning*: hex values are not available in json, so all numbers, both in decimal and hex representations, should be given as strings.

## Acknowledgements and remarks

This repo is originally cloned from [ilya101010/demos](https://github.com/ilya101010/demos).

This project is done under supervision of Boris Baykov and Alexei Ponamarev who I owe a debt of gratitude for all the guidance they provided me during work on this project.