### 📋 reserved-memory 区域表格

| 区域名                        | 大小 (Hex) | 大小 (MB) | 主要作用                               |
| ----------------------------- | ---------- | --------- | -------------------------------------- |
| **hyp_mem**                   | 0x600000   | 6 MB      | Hypervisor 内存 (EL2, QHEE)            |
| **xbl_aop_mem**               | 0x140000   | 1.25 MB   | AOP 电源/时钟控制固件                  |
| **sec_apps_mem**              | 0x1000     | 4 KB      | TrustZone 小型安全应用                 |
| **smem_mem**                  | 0x200000   | 2 MB      | Shared Memory (AP ↔ Modem/DSP 通信)    |
| **removed_mem**               | 0x3900000  | 57 MB     | 标记废弃/不可用区域                    |
| **pil_modem_mem**             | 0x6900000  | 105 MB    | Modem 固件运行区                       |
| **wlan_msa_mem**              | 0x100000   | 1 MB      | WiFi (WLAN MSA) 内存                   |
| **pil_cdsp_mem**              | 0x1e00000  | 30 MB     | CDSP 固件运行内存                      |
| **pil_adsp_mem**              | 0x1e00000  | 30 MB     | ADSP (音频 DSP) 固件运行内存           |
| **pil_ipa_fw_mem**            | 0x10000    | 64 KB     | IPA (IP Accelerator) 固件              |
| **pil_ipa_gsi_mem**           | 0x5000     | 20 KB     | IPA GSI 接口内存                       |
| **pil_gpu_mem**               | 0x2000     | 8 KB      | GPU 固件小块内存                       |
| **video_mem**                 | 0x700000   | 7 MB      | 视频编解码 buffer                      |
| **user_contig_mem**           | 0x1000000  | 16 MB     | 用户态连续内存 pool                    |
| **non_secure_display_memory** | 0x5c00000  | 92 MB     | 普通显示 buffer (非安全)               |
| **qseecom_mem**               | 0x1400000  | 20 MB     | QSEECom 与 TrustZone 通信              |
| **qseecom_ta_mem**            | 0x1000000  | 16 MB     | QSEE Trusted App 内存                  |
| **cdsp_sec_mem**              | 0x1e00000  | 30 MB     | CDSP 安全内存                          |
| **secure_display_memory**     | 0x5c00000  | 92 MB     | 安全显示 (DRM L1 视频播放)             |
| **adsp_mem**                  | 0x800000   | 8 MB      | ADSP 动态 buffer                       |
| **dump_mem**                  | 0x800000   | 8 MB      | 内存 Dump 调试区                       |
| **ramoops_mem**               | 0x200000   | 2 MB      | Ramoops (持久化日志, kernel panic log) |
| **audio_cma_mem**             | 0x1c00000  | 28 MB     | 音频 DMA 连续内存池                    |
| **system_cma**                | 0x2000000  | 32 MB     | 系统 CMA (camera, display 等)          |
| **va_md_mem**                 | 0x1000000  | 16 MB     | 视频分析/运动检测 buffer               |