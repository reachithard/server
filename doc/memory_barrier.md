# 内存知识

## 一、 硬件结构
* 缓存级别
  cache是位于chip-based的电脑组件，能够让cpu获取数据以及指令更方便。速度远远大于内存，为了弥补cpu和内存之间的速度巨大差异。但容量比内存小。cache是SRAM制作，而内存是DRAM。其中cache被分为三级:   
  L1速度最快，但容量最小,通常被集成于cpu内。通常有数据cache，和地址cache.   
  L2速度适中，通常也被集成于cpu内，或者用一条高速总线和cpu相连。
  L3速度慢，通常情况下是多个cpu共用一个。
  // TODO 补图
* cache映射   
   直接映射:   
      将主存里面特定块映射到特定的cache块。
      注意:地址变换算法即查找cache是否miss算法
    ```
    公式为i = j mod c, 即映射到c块（但为了效率有可能是i = j & (c - 1))公式), 还会有一个标记为。这个为映射算法
    地址变换算法为:首先来一个地址，其中包含了tag,index,offset。而cache里面存了valid,tag,以及offset。首先通过index（地址)查找cache line，然后比对tag(cache和地址的)以及valid。匹配则hit，否则miss。然后offset查找字节。因此有一种优化算法是cache line友好。防止MESI协议等，空间换时间。
    ```
    优点：硬件简单，容易实现   
    缺点：命中率低， Cache的存储空间利用率低,会有颠簸情况     
   全相连映射:   
     主存块能到任意cache块。
     ```
     地址变换算法为:首先来一个地址，其中包含了tag,offset。而cache里面存了valid,tag,以及offset。首先遍历（没有index)查找cache line，然后比对tag(cache和地址的)以及valid。匹配则hit，否则miss。然后offset查找字节。
     ```
     优点：命中率较高，Cache的存储空间利用率高   
     缺点：线路复杂，成本高，速度低
   组相连映射:   
     块内全相连映射，块间直接映射。即先分组，组是直接映射，组内随便选。
     ```
    公式为i = j mod c, 即映射到c块（但为了效率有可能是i = j & (c - 1))公式), 还会有一个标记为。这个为映射算法
    地址变换算法为:首先来一个地址，其中包含了tag,index,offset。而cache里面存了valid,tag,以及offset。首先通过index（地址)查找cache line，然后组内遍历比对tag(cache和地址的)以及valid。匹配则hit，否则miss。然后offset查找字节。因此有一种优化算法是cache line友好。防止MESI协议等，空间换时间。
     ```
     硬件较简单，速度较快，命中率较高
   // TODO 补图
* 数据写回策略:   
   直写:   
   最简单的一种写入策略，叫做直。在这个策略里，每一次数据都要写入到主内存里。

   写入前，我们会先去判断数据是否已经在Cache里面了
   如果数据已经在Cache里面，我们就先把数据写入更新到Cache里面，再写入到主内存中
   如果数据不在Cache里面，我们就只更新主内存
   
   性能慢   

   写回策略:  
    在 CPU Cache 的写入策略里，还有一种策略就叫作写回。这个策略里，我们不再是每次都把数据写入到主内存，而是只写到CPU Cache里。只有当CPU Cache里面的数据都要被“替换”的时候，我们才把数据写入到主内存里面去。
    写回策略的过程是这样的:
    如果发现我们要写入的数据，就在CPU Cache里面，那么我们就只是更新CPU Cache里面的数据   
    同时，我们会标记CPU Cache里的这个Block是脏（Drity）的。所谓“脏”，就是指这个时候，我们的CPU Cache里面的这个Block的数据，和主内存是不一致的。   
    如果我们发现，我们要写入的数据所对应的 Cache Block 里，放的是别的内存地址的数据，那么我们就要看一看，那个 Cache Block 里面的数据有没有被标记成脏的。    
    如果是脏的话，我们要先把这个 Cache Block 里面的数据，写入到主内存里面。   
    然后，再把当前要写入的数据，写入到 Cache 里，同时把 Cache Block 标记成脏的。    
    如果 Block 里面的数据没有被标记成脏的，那么我们直接把数据写入到 Cache 里面，然后再把 Cache Block 标记成脏的就好了。    
    在用了写回这个策略之后，我们在加载内存数据到 Cache 里面的时候，也要多出一步同步脏 Cache 的动作。如果加载内存里面的数据到 Cache 的时候，发现 Cache Block 里面有脏标记，我们也要先把 Cache Block 里的数据写回到主内存，才能加载数据覆盖掉Cache   
* TLB   
  TLB即`Translation Lookaside Buffer`;是位于MMU的快表，完成虚拟地址到物理地址的转换。在需要进行虚拟地址到物理地址转换时，MMU会首先查找TLB中是否有缓存，如果hit，立即完成转换，否则，需要到主存的页表（慢表）里面去找，并且将转换关系缓存到TLB中，加速转换。

  ---所以CPU->TLB->CACHE->Memory---
* MMU   
  MMU(Memory Management Unit)，即内存管理单元。完成虚拟地址到物理地址的转换。
* 歧义问题   
  多个进程下，不同进程的相同的虚拟地址可以映射不同的物理地址。这就会造成歧义问题。

  例如，进程A将地址0x2000映射物理地址0x4000。进程B将地址0x2000映射物理地址0x5000。当进程A执行的时候将0x2000对应0x4000的映射关系缓存到TLB中。当切换B进程的时候，B进程访问0x2000的数据，会由于命中TLB从物理地址0x4000取数据。这就造成了歧义。
  解决方法1. flush TLB但消耗大
  解决方法2. 引入标记ASID`Address space id`,进行区分，比对ASID以及虚拟地址，全一致则hit，但如果是内核空间，则还需要判断是不是共用的。所以再引入一个标记位，是不是内核地址，因为内核地址对其他进程是共享的。 
  
## 二、 系统
* 虚拟内存
* 段式存储和页式存储
* 内存结构
* 伙伴分配算法(buddy算法)
* slab分配算法
* 页表
* SMP和NUMA
## 三、 一致性协议
* MESI协议1
## 四、 c++内存序
* volatile
* 内存屏障
* atomic的6种字节序
* 基于atomic的从spinlock->ticketlock->lchlock->mcslock->qspinlock
## 五、 字节对齐
* 字节对齐公式
