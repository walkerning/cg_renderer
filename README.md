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
- [x] 试试mesh物体加入
- [x] 反射, 透射
- [ ] read scene from file方便测试, light, camera, 各个物体.. 看看obj文件, 不行自己定义一个文件格式也行
- [ ] debug效果不对: 还有不少的噪点, 调一些参数看看噪点怎么变.
- [ ] 焦散现象
- [ ] hashing部分加速, 过段时间rebuild hash.
- [ ] monitor the render process
- [ ] 从snapshot开始训练那么写并不对... scale是自动曝光. 现在没拿出来. 而且initial radius重新来科学吗?
- [ ] 并行化算法. 多个MCMC采样并行算? 一起sync的加到真的hitpoint上然后accumulate?
- [ ] 怎么加入depth of field: stochastic photon mapping
- [ ] GPU加速, BVH构建: https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/