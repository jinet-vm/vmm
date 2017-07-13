#!/bin/bash
echo $1
source-highlight -i $1 -o /dev/stdout | echo "<h1>$1</h1>$(cat -) <br>"  | wkhtmltopdf -q - pdfs/$(md5sum $1 | awk '{print $1;}').pdf
