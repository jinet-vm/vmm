# The config file

All neccessary numeric consts used across `asm`, `c` source files and `ld`
scripts should be defined in [config.json](config.json). Here's an example of how some value can be stored:

```json
{
    "const1": "value",
    "const2": "value"
}
```

*Warning*: hex values are not available in json, so all numbers, both in decimal and hex representations, should be given as strings.