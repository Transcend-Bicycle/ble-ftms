# Compile .ino files
arduino-cli compile --fqbn arduino:mbed_nano:nano33ble ftms --output-dir .\ftms\output

# Upload binary files to Arduino Board
 bossac -i -d --port=COM14 -U -i -e -w -v "ftms/output/ftms.ino.bin" -R