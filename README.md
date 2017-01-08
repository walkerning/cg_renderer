Robust Adaptive Photon Tracing using Photon Path Visibility
-----------------------------------------------------------

* [paper链接](http://graphics.ucsd.edu/~henrik/papers/robust_adaptive_progressive_photon_mapping.pdf)

paper里说明的比较清楚的部分:

* Replica exchange: two MC: uniform sampled one, a mutated sampled one. 可以实现big step的path变化, 防止stuck在局部, 防止噪声, 防止较大的初始sample path导致的偏差.
* Adaptive Markov chain MC: learning a global mutation parameter. 自适应学习控制MCMC采样方法中mutation的大小的参数. 此文中使用的目标函数为控制mutation的acceptance ratio.
* Path mutation model based on one global mutation parameter
* Progressive radiance esitimation, radius reduction and flux correction


Other algorithms
--------------

* object intersection: 多种物体的求交算法
* BVH: 求交加速
* Locality Hashing: 判断光子是否落在hitpoint区域内


TODO
----
- [x] object和reader部分的数据结构定义等
- [x] main框架写出来. 这个不难
- [x] env的trace.
- [x] visible关键函数
- [x] mutation scheme实现
- [x] BVH实现
- [x] locality hashing实现
- [x] debug 一个有时候出现的内存访问错误...
- [x] write snapshot...
- [x] read from snapshot
- [ ] debug效果不对: 
    * 感觉有一些像素零零散散的没有值, 有值的像素分布也不太均匀. 
    * 在镜面部分一些地方根本没有任何光线不科学.... 196,608个hitpoint里面在10~100 w个光子都是50000左右个hitpint被打到... 先试试把left墙壁也换成diffuse是什么效果!!! 然后再考虑是不是hash函数在0位置左右有边界情况(还是可以写一个hash的test)
    * 现在这个版本的accept ratio普遍偏低... 
- [ ] read scene from file方便测试
- [ ] monitor the render process

