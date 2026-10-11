/*
 * cmsisrtx_config.c
 *
 * cmsisrtx/embot: run-time configuration of what the original RTX fixes at build time with RTX_Config.h.
 *
 * It is NOT compiled by itself: cfg/cmsisrtx_lib.c includes it right after rtx_lib.c, so that it is in the same
 * translation unit (same RTX_Config.h, same CMSISRTX_RUNTIME_CONFIG) of osRtxConfig and of the static objects of
 * rtx_lib.c (the control blocks of the idle and timer threads and of the timer queue, that we reuse).
 *
 * What the kernel reads, and when:
 * - osKernelInitialize(): ISR queue, memory pools. In this lib it runs before main(), in _platform_post_stackheap_init(),
 *   because the C library needs the RTX mutexes since its own initialisation.
 * - osKernelStart(): tick frequency, idle thread, timer thread, timer queue. These can be changed in between.
 * The ISR queue was already copied by the initialisation into osRtxInfo: it is patched there too (it must be empty).
 */

#if (CMSISRTX_RUNTIME_CONFIG != 0)

static osThreadAttr_t cmsisrtx_idle_attr;
static osThreadAttr_t cmsisrtx_timer_attr;
static osMessageQueueAttr_t cmsisrtx_timer_mq_attr;

static int cmsisrtx_config_stackok(const void *mem, uint32_t size)
{
    if((NULL == mem) && (0U == size))
    {   // nothing to change
        return 1;
    }
    return (NULL != mem) && (0U == ((uintptr_t)mem & 7U)) && (0U == (size & 7U)) && (size >= 72U);
}

int32_t cmsisrtx_config_set(const cmsisrtx_config_t *c)
{
    if(NULL == c)
    {
        return CMSISRTX_CONFIG_ERR_PARAM;
    }

    const uint8_t state = osRtxInfo.kernel.state;
    if((osRtxKernelInactive != state) && (osRtxKernelReady != state))
    {
        return CMSISRTX_CONFIG_ERR_STATE;
    }

    // -- 1. validate everything, so that nothing is changed if something is wrong

    if((0U != c->tick_freq) && ((SystemCoreClock / c->tick_freq) > (1UL << 24)))
    {   // the SysTick has a 24 bit reload register
        return CMSISRTX_CONFIG_ERR_PARAM;
    }
    if((0U != c->tick_freq) && (0U == (SystemCoreClock / c->tick_freq)))
    {
        return CMSISRTX_CONFIG_ERR_PARAM;
    }

    if((0 == cmsisrtx_config_stackok(c->idle_stack_mem, c->idle_stack_size)) ||
       (0 == cmsisrtx_config_stackok(c->timer_stack_mem, c->timer_stack_size)))
    {
        return CMSISRTX_CONFIG_ERR_PARAM;
    }

    if((0U != c->timer_priority) && ((c->timer_priority <= (uint32_t)osPriorityLow) || (c->timer_priority >= (uint32_t)osPriorityISR)))
    {
        return CMSISRTX_CONFIG_ERR_PARAM;
    }

    const int mq_change = (NULL != c->timer_mq_mem) || (0U != c->timer_mq_mem_size) || (0U != c->timer_mq_msgs);
    if(0 != mq_change)
    {
        if((NULL == c->timer_mq_mem) || (0U != ((uintptr_t)c->timer_mq_mem & 3U)) || (0U == c->timer_mq_msgs) ||
           (c->timer_mq_mem_size < osRtxMessageQueueMemSize(c->timer_mq_msgs, sizeof(osRtxTimerFinfo_t))))
        {
            return CMSISRTX_CONFIG_ERR_PARAM;
        }
    }

    const int isr_change = (NULL != c->isr_queue_mem) || (0U != c->isr_queue_size);
    if(0 != isr_change)
    {
        if((NULL == c->isr_queue_mem) || (0U != ((uintptr_t)c->isr_queue_mem & 3U)) ||
           (c->isr_queue_size < 4U) || (c->isr_queue_size > 0xFFFFU))
        {
            return CMSISRTX_CONFIG_ERR_PARAM;
        }
        if((osRtxKernelReady == state) && (0U != osRtxInfo.isr_queue.cnt))
        {
            return CMSISRTX_CONFIG_ERR_STATE;
        }
    }

    // -- 2. apply

    if(0U != c->tick_freq)
    {
        osRtxConfig.tick_freq = c->tick_freq;
    }

    if(NULL != c->idle_stack_mem)
    {
        cmsisrtx_idle_attr = *osRtxConfig.idle_thread_attr;         // name, attr_bits, control block, priority as before
        cmsisrtx_idle_attr.stack_mem = c->idle_stack_mem;
        cmsisrtx_idle_attr.stack_size = c->idle_stack_size;
        osRtxConfig.idle_thread_attr = &cmsisrtx_idle_attr;
    }

    if((NULL != c->timer_stack_mem) || (0U != c->timer_priority))
    {
        cmsisrtx_timer_attr = *osRtxConfig.timer_thread_attr;
        if(NULL != c->timer_stack_mem)
        {
            cmsisrtx_timer_attr.stack_mem = c->timer_stack_mem;
            cmsisrtx_timer_attr.stack_size = c->timer_stack_size;
        }
        if(0U != c->timer_priority)
        {
            cmsisrtx_timer_attr.priority = (osPriority_t)c->timer_priority;
        }
        osRtxConfig.timer_thread_attr = &cmsisrtx_timer_attr;
    }

    if(0 != mq_change)
    {
        cmsisrtx_timer_mq_attr = *osRtxConfig.timer_mq_attr;
        cmsisrtx_timer_mq_attr.mq_mem = c->timer_mq_mem;
        cmsisrtx_timer_mq_attr.mq_size = c->timer_mq_mem_size;
        osRtxConfig.timer_mq_attr = &cmsisrtx_timer_mq_attr;
        osRtxConfig.timer_mq_mcnt = c->timer_mq_msgs;
    }

    if(0 != isr_change)
    {
        osRtxConfig.isr_queue.data = c->isr_queue_mem;
        osRtxConfig.isr_queue.max = (uint16_t)c->isr_queue_size;
        if(osRtxKernelReady == state)
        {   // already copied by osKernelInitialize()
            osRtxInfo.isr_queue.data = c->isr_queue_mem;
            osRtxInfo.isr_queue.max = (uint16_t)c->isr_queue_size;
            osRtxInfo.isr_queue.in = 0U;
            osRtxInfo.isr_queue.out = 0U;
        }
    }

    return CMSISRTX_CONFIG_OK;
}

#else

int32_t cmsisrtx_config_set(const cmsisrtx_config_t *c)
{
    (void)c;
    return CMSISRTX_CONFIG_UNSUPPORTED;
}

#endif // CMSISRTX_RUNTIME_CONFIG
