## NON-HLOS.bin构建
* **将签名后的镜像原路径覆盖回代码基线**
提供的签名zip包，除了sectool文件，其他的都保留了源路径
![image-20250717181547917](C:\Users\hongjian.liang\AppData\Roaming\Typora\typora-user-images\image-20250717181547917.png)


* **脚本路径**
``` shell
$(pwd)/project/nonhlos/Divar.LA.3.0.1/common/build/build.py
#############################参数说明########################################
build.py [--wflow_filter= <filter1, filter2 ...n> --nonhlos, --hlos, --cmm, --imf, --clean]
   Examples:
build.py --nonhlos  (generates NON_HLOS.bin alone )
build.py --hlos     (generates sparse images if rawprogram0.xml exists)
build.py --wflow_filter= <filter1, filter2.... > ( Execute given filter steps compilation time)
build.py            (generates NON-HLOS.bin,sparse images and other binaries define work flow step)
build.py --imf      (Ignore file path validation. Generates NON-HLOS.bin,sparse images and other binaries define work flow step)
build.py --clean    (Remove all generated binaries created at the time of meta compilation)
```
* <span style="color:red">**运行脚本**</span>

``` shell
python build.py --imf 2>&1|tee generate_non_hlos.log
```
NON-HLOS.bin生成在
`$(pwd)/project/nonhlos/Divar.LA.3.0.1/common/build/ufs/bin/asic/NON-HLOS.bin`

* **构建NON-HLOS.bin的规则描述**

path：`$(pwd)/project/nonhlos/Divar.LA.3.0.1/contents.xml`
``` xml
    <step filter="non_hlos" type="delete">
      <destn>./ufs/bin/$[flavor:.]/NON-HLOS.bin</destn>
    </step>
    <step filter="non_hlos" type="exec">
      <params>python @tool_name -a @attributes -c @destn --ps @partition_size --ss 4096 -f $[flavor:.] --fatgen @fatgen_loc --fatadd @fatadd_loc</params>
      <tool_name>fat_creation.py</tool_name>
      <attributes>&quot;(fat_file)||(fat_file_ufs)&quot;</attributes>
      <destn>./ufs/bin/$[flavor:.]/NON-HLOS.bin</destn>
      <partition_size>@getPartitionSize(&quot;./../config/ufs/partition.xml&quot;,&quot;modem_a&quot;)</partition_size>
      <fatgen_loc>@root_dir + common/config/storage/fatgen.py</fatgen_loc>
      <fatadd_loc>@root_dir + common/config/storage/fatadd.py</fatadd_loc>
      <root_dir>$[build:common]</root_dir>
    </step>
```
