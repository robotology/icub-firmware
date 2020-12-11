Encoder drivers overview
========================

The PMC board uses two kind of encoders

- The LR17 motor embedded absolute SPI encoder
- Two LL06 motor embedded incemental encoders

There are two (little) drivers:

- [`qe_encoder.c`](./qe_encoder.c) for reading incremental encoders
- [`lr17_encoder.c`](./lr17_encoder.c) for reading the LR17 absolute encoder

ARM demo
--------

The drivers (i.e. the drivers' directory) are stored inside the tree of an ARM demo program (generated with CubeMX).
The demo application tries to periodically read all encoders and to print their values. Note that this is Currently **completely untested** (however it compiles..).

QE APIs
-------

`bool qe_encoder_init(qe_encoder_t *e, qe_encoder_cfg_t *c)`

Initalize the QE encoder driver. The first parameter is a motor handle which is filled by this funcions and should be used to call any other API. The second parameter is the motor configuration object. Currently it just carries the STM TIMER handle.

`bool qe_encoder_get(qe_encoder_t *e, int *val)`

Reads the current QE encoder count. It fills the variable passed as 2nd parameter

ABS APIs
--------

`bool lr17_encoder_init()`
Initalize the LR17 absolute encoder driver.


`bool lr17_encoder_acquire(void(*cb)(void *arg), void *arg)`
Starts the encoder acquisition. It returns immediately. Optionally, once the HW returned the new encoder value the callback specified as parameters is called . The 2nd argument is passed to the callback as its only argument. The callback can be NULL;


`bool lr17_encoder_get(int *angle)`
This functions fills the variable passed as 1st parameters with the last econder available reading.
