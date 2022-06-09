## 内存管理

- Allocator（内存分配器）：固定分区（类型）分配
  - 每个`block`中有一块大小为类型`T`字节数的`buffer`，还有一个`next`指针指向下一个`block`。
  - 每个`chunk`中有`blocks_per_chunk`个`block`，还有一个`next`指针指向下一个`chunk`。每创建一个`chunk`，其中前`blocks_per_chunk-1`个`block`的`next`指针都指向下一个`block`。
  - 维护一个`chunk`单向链表头结点和空闲`block`单向链表头结点。
  - 分配内存时，若空闲`block`头结点不为空，则将其分配，空闲`block`头结点指向其下一个`block`，否则创建一个新的`chunk`，采用头插法插入当前`chunk`头结点之前，将其中的第一个`block`分配，第二个`block`作为空闲`block`头结点。
  - 归还内存时，采用头插法插入当前空闲`block`头结点之前。
- MemoryPool（内存池）：动态分区分配
  - 使用`tag`记录每块`buffer`的信息，`tag`大小为`sizeof(size_t)`个字节，格式为: size prev_free(1b) free(1b)，最低位表示当前`block`是否空闲，次低位表示前一个`block`是否空闲，其余表示当前`block`的字节数，包括`tag`和`buffer`。将当前`block`头部的`tag`指针加上`tag`中记录的`size`个字节即可访问下一个`block`头部的`tag`
  - 每个`block`头部应有一个`tag`，回收内存时将指针回退`tag`所占字节数个字节即可访问到它。
  - 空闲`block`通过查看`prev_free`判断前一个`block`是否空闲，若空闲则需要与其合并。为了能访问到前一个空闲`block`，需要给空闲`block`的尾部也加上`tag`，它的内容与头部的`tag`相同。这样将当前`block`头部的`tag`指针回退`tag`所占字节数个字节即可访问前一个空闲`block`尾部的`tag`。因此对于空闲`block`，它的`tag`中记录的`size`为两倍`tag`所占字节数加上一个`buffer`的大小。 非空闲`block`的尾部无需加`tag`，因为若`prev_free`不为`true`时无需访问前一个非空闲的`block`。
  - 每个`chunk`的开头需要储存下个`chunk`的地址，供释放内存时用；结尾也需要储存下个`chunk`的地址，供最后一个`block`跳转下个`chunk`的第一个`block`时用。为了让`block`能够识别出到达了`chunk`的尾部，需要在最后一个`block`之后留出一个`tag`的空间，置为0，当访问到此全0`tag`就知道到达了`chunk`的尾部。第一个`block`之前无需特殊标记，因为它的`prev_free`在新`chunk`创建时就已经设置为`false`，并且由于它前面没有`block`了，它的`prev_free`肯定不会改为`true`，也就不会访问到前面的内存。
  - 无论`block`是否空闲，它的`buffer`的大小都为`tag`中记录的`size`减去一个`tag`所占字节数。对于空闲`block`，当其被分配时，即转为非空闲`block`时，其尾部的`tag`的空间也作为`buffer`。
  - 分配内存采用循环首次适应算法。使用一个变量`curr_block_`记录当前`block`的地址，下次继续请求分配内存时从`curr_block_`开始遍历。当遍历到最后一个`block`时，若下个`chunk`的地址不为空，则移动`curr_block_`至下个`chunk`的第一个`block`，否则移动`curr_block_`至第一个`chunk`的第一个`block`。当`curr_block_`回到初始位置时，说明没找到符合条件的`block`，需要创建新的`chunk`。
