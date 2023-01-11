# OpenConsult
An open-source, cross-platform C++ library for reading vehicle ECU diagnostic and performance data via the Consult protocol - a pre-cursor to standardized OBD protocols, used by Nissan from the late 1980s to the early 2000s.

After finding no cross-platform tooling to read my ECU, and few high-quality open-source implementations, I felt it would be beneficial for the community to develop something.

OpenConsult will comprise of:
* A central library, exposing an API to connect and interact with an ECU.
* A command-line tool to read fault codes and capture data logs.
* A graphical tool to read fault codes, capture data logs and display a dashboard - suitable for in-vehicle display. This will be implemented as a web-app, leaving open the possibility of repackaging as an Android/iOS app.

OpenConsult will support Windows, Mac and Linux; x86 and Arm. It will be written to run on low-power SBCs (e.g. a Raspberry Pi), with a view to deploying in-vehicle - either to display the dashboard directly, or drive mechanical gauges using the library.

## Status
OpenConsult is an active work in progress and at an extremely early stage. I do not yet consider it high enough quality to recommend its usage on other people's cars. I'll add usage instructions when it reaches a point I feel comfortable with other people using it.

## Supported Vehicles
OpenConsult initially only implements the Consult I protocol. This means it only supports Nissan vehicles manufactured from 1989-2000 - any that have a gray 14-pin Consult I connector. It will not work with vehicles with the 16-pin OBDII connector, as used on their more recent vehicles.

I only have access to an S14 and an R34 (both JDM models) to test on. As such these are the only vehicles I can guarantee it supports robustly.

Any of the USB 14-pin USB connectors should work. I bought mine from eBay for £16 (try searching for "14 pin Nissan USB interface" - they often come with some pretty unusable Windows software provided on an equally unusable mini-CD). Bluetooth connectors are not (yet) supported.

## Contributing
If anyone is interested in either contributing directly or helping increase the list of known-supported vehicles - please get in touch!

## License
OpenConsult is licensed under the GPLv3 license. See [LICENSE](LICENSE) for more information.
