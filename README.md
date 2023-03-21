# DPDK-Based Message Broker

## ABSTRACT
The work in this project presents a message broker that implements the Publisher/Subscriber model. The idea is to create a high performance, MQTT-like, message dissemination platform for IoT networks in the high speed 5G networking era. The broker system uses ANS-DPDK to enhance its performance by going around the Linux’s TCP/IP network stack’s method of handling small TCP connections, which is insufficient for handling an MQTT compliant system. Two versions of this system were created, one uses Linux sockets for communication and the other uses ANS-DPDK sockets. Several experiments were conducted to test the effect of implementing a Broker with ANS-DPDK. With the ANS-DPDK’s sockets, the system achieves 82% higher throughput and around 20% less end-to-end latency.
