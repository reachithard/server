# 内存知识

## 一、 硬件结构
* 缓存级别
  cache是位于chip-based的电脑组件，能够让cpu获取数据以及指令更方便。速度远远大于内存，为了弥补cpu和内存之间的速度巨大差异。但容量比内存小。cache是SRAM制作，而内存是DRAM。其中cache被分为三级:   
  L1速度最快，但容量最小,通常被集成于cpu内。通常有数据cache，和地址cache.   
  L2速度适中，通常也被集成于cpu内，或者用一条高速总线和cpu相连。
  L3速度慢，通常情况下是多个cpu共用一个。

* cache映射   
   直接映射:   
   全相连映射:   
   组相连映射:
* iommu
## 二、 系统
* 虚拟内存
* 段式存储和页式存储
* 内存结构
* 伙伴分配算法(buddy算法)
* slab分配算法
* TLB
* 页表
* SMP和NUMA
## 三、 一致性协议
* MESI协议
## 四、 c++内存序
* volatile
* 内存屏障
* atomic的6种字节序
* 基于atomic的从spinlock->ticketlock->lchlock->mcslock->qspinlock
## 五、 字节对齐
* 字节对齐公式
