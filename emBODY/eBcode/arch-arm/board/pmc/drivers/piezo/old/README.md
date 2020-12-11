The PMC board has three piezo motors, driven by three DACs, that are logically independant, and it would be great (from the FW point of view) to control them with a single, generic, driver that can be instantiated three times (as Marco suggests).

I've tried myself to write some code to do that (in a pc-emulated environment, since I have no HW); I've tried to extract the common logic from Giorgio's code and to rewrite it in a instance-agnostic way, so that it could be instantiated one time for each motor, with a nice API.

.. I failed. It turned out that the three motor controllers cannot run on their own; they have to be handled as one single thing.

The HW has hard requirements about how they are handled: they must be prepared all togheter and started in proper order; there is a "master" one that must be running otherwise the other ones cannot work. The three communication BUSes works togheter and they must be synchronized.

This is because of the following quirks:

One master SPI peripheral controls one of the motors. It works almost normally, but it can TX just 16-bit words. An auxiliary circuit (that must be properly reset before starting the communication) prescales the nCS signal by killing one high pulse every two. In this way two 16-bit words appare to be merged in one single bus transaction to the slave DAC.

The other two SPI peripheral, while still logically working as a master wrt their DACs, are actually configured as slaves. Their MISO feeds the DAC MOSI. The first SPI interface (master) is the only one that generates the clock; such  clock is fed also into the SPI SLAVE interfaces, which are also configured to ignore the CS.

As a result, when the master SPI interface runs, it feeds the clock in the slave interfaces, that start to shift out the data on their MISO pin. All three DACs are fed with the master clock and master nCS (prescaled by two).

It is essential that:

- the SPI slave interfaces (and their DMAs) are ready to TX (i.e. the HAL start TX API has been invoked) when the master SPI interface is started, otherwise the slave interfaces would start to TX at a random time, without being in sync with the prescaled CS signal.

- the SPI master interface is kept running as long as any SPI interface need to run (i.e. you cannot stop the 1st motor when the 2nd or 3rd needs to run)

- you cannot stop/start any DMA alone indeed. You need to start and stop all of them toghether

For those reason it appares that it is not possible to handle each motor per se. It is possible to separate some code portion (i.e. use separate ISR handlers to refill the buffers, instead of refilling all three buffers in the SPI master DMA ISR), but still this is not enough to get the three separated instances.

I like to think about the PMC hardware as a cuckoo clock: It's really pretty, but all that small gears and wheels must run precisely and perfectly sinchronized, otherwise we get no bird....

At this points I could try to propose discussing some possible solutions:

**1a.** Ignore this problem, go straight with the original idea and pretend the caller to take care of calling things in proper order or so.

IMO this is dangerous and bug-prone: we are pretending to expose a nice and simple API that simply does not work (unless used in a very specific, and sneaky, way).

It's really likely that the first time someone has to modify something in future (and she/he doesn't remember/know about this magic things) all become broken for no apparent reasons.. This is the highway for long bug-hunting nights..

**1b.** Ignore this problem, kill the proposed API, use the single motor driver that controls three motors.. things like `start()` and `stop()` methods could be emulated by the motor driver the best as it can. (i.e. require to stop a motor that cannot be really stopped could end up update the DMA buffer to stay still, letting the DMA run anyway and disable the ISR to save CPU resources.

**2.** split the motor driver in two parts

- one that drives the DAC and represent the motor object
- one that manages the three DMAs

Now there are no hidden traps anymore for the caller, which still sometime needs to deal with the DMA manager as a single object, and explicitlty allocate it and the three other motor objs, but while running there are separated motor object to control motors without any quirk.

There would be a "master start" operation  (which can be also buried in the initialization) and a "master stop" operation (which can be also missing) in the DMA part, and there would be a `stop()` and `start()` method on the motor object that would emulate the operations as explained in 1b).

Note that the DMA object allocates the DMA buffers (because it needs to start the DMA, in correct order, early for all motors), while the motor objects just fill them. This is OK, but the (three) configuration object (which probably includes the DMA buffer len) must be passed to the DMA manager, rather than to the (three) DAC objects.

**3.** let the motor driver be one single entity (that controls three motors) but introduce some entity that exposes the originally proposed API. This can be done if the three motor object are not instantiated by the caller, but a kind of factory it's used. The factory returns the three motor objects; each one expose the wanted API, but they are just facades that wraps back to the single motor driver. The factory creates the three objects toghether and it entangles them, but no one notices..)


It could be something like this:


```

/*
 * the PMC motor manager/driver. Manages 3 motors, and knows about their dependancies and quirk.
 * This is basically built around Giorgio's code...
 * Singleton in our design.
 */
pmc_motor p;

motor_user(...)
{
	pizo_motor_t pz1, pz2, pz3;

	/* the PMC motor driver give us objects representing the interface to each motor */
	pmc_motor_create(&p, &pz1, &pz2, &pz3)

	/* we use them as independent instances */
	piezo_init(pz1, cfg1);
	piezo_init(pz2, cfg2);
	piezo_init(pz3, cfg3);


	/* no worries about anything */
	piezo_start(pz1);
	piezo_set_v(pz1, v);

	piezo_start(pz2);
	piezo_stop(pz1);
	piezo_start(pz3);
	piezo_set_v(pz3, vvv);

...
}

/* the piezo motor objects wraps onto the PMC motor driver object */

int piezo_start(piezo_handle_t *p)
{
	pmc_motor_start(p->pmc, p->id)
}

```


.. *But..* note that in C this is not so different by directly using the single motor driver as proposed in 1b):


```
motor_user(...)
{

	/* the PMC motor driver is initialized */
	pmc_motor_init(&p)

	/* we use it specifying the motor id */
	pmc_motor_cfg(pz, 1, cfg1);
	pmc_motor_cfg(pz, 2, cfg2);
	pmc_motor_cfg(pz, 3, cfg3);


	/* no worries about anything */
	pmc_motor_start(pz, 1);
	pmc_motor_set_v(pz, 1, v);

	pmc_motor_start(pz, 2);
	pmc_motor_stop(pz, 1);
	pmc_motor_start(pz, 3);
	pmc_motor_set_v(pz, 3, vvv);

...
}

```


In C++ the difference could be more interesting, but I don't know yet (I rarely use C++, and I tend to make it to degenerate to C very quickly).

In any case, Giorgio's code need to be modified in order to handle `start()` and `stop()` methods properly with the "emulation" discussed above.

Other than that, in Giorgio's code a interpolation could be added. It seems quite complete for the rest.

As a last note, I saw that the shared table is propsed to be a shared object with a `get()` method that calls some `memmove()`. This method would be called tons of time at each interrupt (which are very frequent and critical) so I wuold stick to something more efficent. I think that a simple array should do the job..
