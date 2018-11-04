 OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
MEMORY
    {
    FLASH (rx) : ORIGIN = (0x0 + 0), LENGTH = (512*1K - 0)
    SRAM (wx) : ORIGIN = 0x20000000, LENGTH = (64 * 1K)
    IDT_LIST (wx) : ORIGIN = (0x20000000 + (64 * 1K)), LENGTH = 2K
    }
ENTRY("__start")
SECTIONS
    {
   
 _image_rom_start = (0x0 + 0);
    text :
 {
 . = 0x0;
 _vector_start = .;
 KEEP(*(.exc_vector_table))
 KEEP(*(".exc_vector_table.*"))
 KEEP(*(.gnu.linkonce.irq_vector_table))
 KEEP(*(.openocd_dbg))
 KEEP(*(".openocd_dbg.*"))

 KEEP(*(.kinetis_flash_config))
 KEEP(*(".kinetis_flash_config.*"))
 KEEP(*(.gnu.linkonce.sw_isr_table))
 _vector_end = .;
 _image_text_start = .;
 *(.text)
 *(".text.*")
 *(.gnu.linkonce.t.*)
 } > FLASH
 _image_text_end = .;
 .ARM.exidx :
 {
 __exidx_start = .;
 *(.ARM.exidx* gnu.linkonce.armexidx.*)
 __exidx_end = .;
 } > FLASH
 _image_rodata_start = .;
 devconfig () :
 {
  __devconfig_start = .;
  *(".devconfig.*")
  KEEP(*(SORT(".devconfig*")))
  __devconfig_end = .;
 } > FLASH
 net_l2 () :
 {
  __net_l2_start = .;
  *(".net_l2.init")
  KEEP(*(SORT(".net_l2.init*")))
  __net_l2_end = .;
 } > FLASH
 log_const_sections () : ALIGN_WITH_INPUT
 {
  __log_const_start = .;
  KEEP(*(SORT(.log_const_*)));
  __log_const_end = .;
 } > FLASH
 log_backends_sections () : ALIGN_WITH_INPUT
 {
  __log_backends_start = .;
  KEEP(*(".log_backends"));
  __log_backends_end = .;
 } > FLASH
    rodata :
 {
 *(.rodata)
 *(".rodata.*")
 *(.gnu.linkonce.r.*)
 . = ALIGN(4);
 } > FLASH
 _image_rodata_end = .;
 _image_rom_end = .;
   
   
_app_data_align = 32;
. = ALIGN(32);
 app_smem () :
 {
  . = ALIGN(4);
  _image_ram_start = .;
  _app_smem_start = .;
KEEP(*(SORT(data_smem_[_a-zA-Z0-9]*)))
  _app_smem_end = .;
  _app_smem_size = _app_smem_end - _app_smem_start;
  . = ALIGN(4);
 } > SRAM AT> FLASH
 _app_smem_rom_start = LOADADDR(app_smem);
    bss (NOLOAD) : ALIGN_WITH_INPUT
 {
        . = ALIGN(4);
 __bss_start = .;
 __kernel_ram_start = .;
 *(.bss)
 *(".bss.*")
 *(COMMON)
 *(".kernel_bss.*")
 __bss_end = ALIGN(4);
 } > SRAM AT> SRAM
    noinit (NOLOAD) :
        {
        *(.noinit)
        *(".noinit.*")
 *(".kernel_noinit.*")
        } > SRAM
    datas : ALIGN_WITH_INPUT
 {
 __data_ram_start = .;
 *(.data)
 *(".data.*")
 *(".kernel.*")
 } > SRAM AT> FLASH
    __data_rom_start = LOADADDR(datas);
 initlevel () : ALIGN_WITH_INPUT
 {
  __device_init_start = .; __device_PRE_KERNEL_1_start = .; KEEP(*(SORT(.init_PRE_KERNEL_1[0-9]))); KEEP(*(SORT(.init_PRE_KERNEL_1[1-9][0-9]))); __device_PRE_KERNEL_2_start = .; KEEP(*(SORT(.init_PRE_KERNEL_2[0-9]))); KEEP(*(SORT(.init_PRE_KERNEL_2[1-9][0-9]))); __device_POST_KERNEL_start = .; KEEP(*(SORT(.init_POST_KERNEL[0-9]))); KEEP(*(SORT(.init_POST_KERNEL[1-9][0-9]))); __device_APPLICATION_start = .; KEEP(*(SORT(.init_APPLICATION[0-9]))); KEEP(*(SORT(.init_APPLICATION[1-9][0-9]))); __device_init_end = .;
 } > SRAM AT> FLASH
 initlevel_error () : ALIGN_WITH_INPUT
 {
  KEEP(*(SORT(.init_[_A-Z0-9]*)))
 }
 ASSERT(SIZEOF(initlevel_error) == 0, "Undefined initialization levels used.")
 initshell () : ALIGN_WITH_INPUT
 {
  __shell_module_start = .; KEEP(*(".shell_module_*")); __shell_module_end = .; __shell_cmd_start = .; KEEP(*(".shell_cmd_*")); __shell_cmd_end = .;
 } > SRAM AT> FLASH
 log_dynamic_sections () : ALIGN_WITH_INPUT
 {
  __log_dynamic_start = .;
  KEEP(*(SORT(.log_dynamic_*)));
  __log_dynamic_end = .;
 } > SRAM AT> FLASH
 _static_thread_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _static_thread_data_list_start = .;
  KEEP(*(SORT("._static_thread_data.static.*")))
  _static_thread_data_list_end = .;
 } > SRAM AT> FLASH
 _k_timer_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_timer_list_start = .;
  KEEP(*(SORT("._k_timer.static.*")))
  _k_timer_list_end = .;
 } > SRAM AT> FLASH
 _k_mem_slab_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_mem_slab_list_start = .;
  KEEP(*(SORT("._k_mem_slab.static.*")))
  _k_mem_slab_list_end = .;
 } > SRAM AT> FLASH
 _k_mem_pool_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  KEEP(*(SORT("._k_memory_pool.struct*")))
  _k_mem_pool_list_start = .;
  KEEP(*(SORT("._k_mem_pool.static.*")))
  _k_mem_pool_list_end = .;
 } > SRAM AT> FLASH
 _k_sem_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_sem_list_start = .;
  KEEP(*(SORT("._k_sem.static.*")))
  _k_sem_list_end = .;
 } > SRAM AT> FLASH
 _k_mutex_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_mutex_list_start = .;
  KEEP(*(SORT("._k_mutex.static.*")))
  _k_mutex_list_end = .;
 } > SRAM AT> FLASH
 _k_alert_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_alert_list_start = .;
  KEEP(*(SORT("._k_alert.static.*")))
  _k_alert_list_end = .;
 } > SRAM AT> FLASH
 _k_queue_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_queue_list_start = .;
  KEEP(*(SORT("._k_queue.static.*")))
  _k_queue_list_end = .;
 } > SRAM AT> FLASH
 _k_stack_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_stack_list_start = .;
  KEEP(*(SORT("._k_stack.static.*")))
  _k_stack_list_end = .;
 } > SRAM AT> FLASH
 _k_msgq_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_msgq_list_start = .;
  KEEP(*(SORT("._k_msgq.static.*")))
  _k_msgq_list_end = .;
 } > SRAM AT> FLASH
 _k_mbox_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_mbox_list_start = .;
  KEEP(*(SORT("._k_mbox.static.*")))
  _k_mbox_list_end = .;
 } > SRAM AT> FLASH
 _k_pipe_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_pipe_list_start = .;
  KEEP(*(SORT("._k_pipe.static.*")))
  _k_pipe_list_end = .;
 } > SRAM AT> FLASH
 _k_work_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_work_list_start = .;
  KEEP(*(SORT("._k_work.static.*")))
  _k_work_list_end = .;
 } > SRAM AT> FLASH
 _k_task_list () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_task_list_start = .;
  *(._k_task_list.public.*)
  *(._k_task_list.private.*)
  _k_task_list_idle_start = .;
  *(._k_task_list.idle.*)
  KEEP(*(SORT("._k_task_list*")))
  _k_task_list_end = .;
 } > SRAM AT> FLASH
 _k_event_list () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _k_event_list_start = .;
  *(._k_event_list.event.*)
  KEEP(*(SORT("._k_event_list*")))
  _k_event_list_end = .;
 } > SRAM AT> FLASH
 _k_memory_pool () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  *(._k_memory_pool.struct*)
  KEEP(*(SORT("._k_memory_pool.struct*")))
  _k_mem_pool_start = .;
  *(._k_memory_pool.*)
  KEEP(*(SORT("._k_memory_pool*")))
  _k_mem_pool_end = .;
 } > SRAM AT> FLASH
 _net_buf_pool_area () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  _net_buf_pool_list = .;
  KEEP(*(SORT("._net_buf_pool.static.*")))
 } > SRAM AT> FLASH
 net_if () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __net_if_start = .;
  *(".net_if.*")
  KEEP(*(SORT(".net_if.*")))
  __net_if_end = .;
 } > SRAM AT> FLASH
 net_if_dev () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __net_if_dev_start = .;
  *(".net_if_dev.*")
  KEEP(*(SORT(".net_if_dev.*")))
  __net_if_dev_end = .;
 } > SRAM AT> FLASH
 net_l2_data () : ALIGN_WITH_INPUT SUBALIGN(4)
 {
  __net_l2_data_start = .;
  *(".net_l2.data")
  KEEP(*(SORT(".net_l2.data*")))
  __net_l2_data_end = .;
 } > SRAM AT> FLASH
    __data_ram_end = .;
    _image_ram_end = .;
    _end = .;
    __kernel_ram_end = 0x20000000 + (64 * 1K);
    __kernel_ram_size = __kernel_ram_end - __kernel_ram_start;
   
/DISCARD/ :
{
 KEEP(*(.irq_info))
 KEEP(*(.intList))
}
    }