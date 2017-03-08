# demos

![Aristotle](aristotle.gif)

Demo operating system. **So** much work to do.

## Config file

All neccessary numeric consts used across `asm`, `c` source files and `ld`
scripts should be defined in [config.json](config.json). Here's an example of how some value can be stored:

```json
{
    "name":
    {
        "value": "0x1010010",
        "files": ["kernel_config.h", "bootloader_config.inc"]
    }
}
```

*Warning*: hex values are not available in json, so all numbers, both in decimal and hex representations, should be given as strings.


Three file extensions are available in `files`:
- `*.h` - these are C/C++ headers
- `.inc` - these are `fasm` macro files
- `.ld` - these are ld scripts, values will be appended to file's end
