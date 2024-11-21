# ESP-NOW-OSC

this is a work in progress and not yet usable


## TODO
* test virtual network card configuration
* serialize osc message, checkout how it is done in oscmessage::send
* parse incoming osc data, oscmessage::fill does probably the job
* send out to the connected computer, hopefully networkdevice::getgateway can be used, would like to avoid having to set an ip address
* handle messages that are larger than max esp now data size