# cairo-examples

如果你不想费了劲的在 vs 下编译 cario(依赖库有点多)
可以从 libcairo-2.dll 导出 libcairo-2.lib 使

**run_dll2lib.bat**

```
pexports libcairo-2.dll > libcairo-2.def
call "%VS80COMNTOOLS%\vsvars32.bat"
lib /machine:i386 /def:libcairo-2.def
```


## src/test_wtl_cario

基本的双缓冲方式使用 cario


## src/test_double_buffer_cario_gdiplus

cario 和 gdi+ 同时使用的双缓冲方式绘制，强制使用 32 位缓冲区位图