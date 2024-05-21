缓存常见策略：

# LRU
最近最少策略LRU（Least Recently Used），简而言之，高频访问的放在链表头部，从而能够淘汰掉进入链表时间最长且没有二次访问过的元素。

淘汰很久没有被访问过的数据，以最近一次访问时间为参考



维护一个有序单链表，越靠近链表尾部的结点是最早之前访问的，

当有一个新的数据被访问时，我们从链表头开始顺序遍历链表，

1. 如果此数据之前被换存在链表中来，我们遍历得到这个数据的结点，并将其从原来的位置删除，然后再插入到链表的头部。
2. 如果此数据没有换存在链表中：

1. 如果此时缓存未满，则直接将节点插入到链表的头部；
2. 如果此时缓存已满，则链表尾结点删除，将新的数据结点插入链表的头部。



## [146. LRU 缓存](https://leetcode.cn/problems/lru-cache/)

```java
class LRUCache {
  private static class Node {
    private int key;
    private int value;
    Node prev;
    Node next;
    Node(int k, int v) {
      this.key = k;
      this.value = v;
    }
  }
  private final int capacity;
  private final Node dummy = new Node(0, 0);
  private final Map<Integer, Node> keyToNodeMap = new HashMap<>();

  public LRUCache(int capacity) {
    this.capacity = capacity;
    // 循环双向链表
    dummy.prev = dummy;
    dummy.next = dummy;
  }

  public int get(int key) {
    Node node = getNode(key);
    return node == null ? - 1 : node.value;
  }

  // 如果此数据存在链表中，则更新数据；不存在时看链表是否满，如果满则删除尾部节点，然后插入头节点，未满则直接插入头节点。
  public void put(int key, int value) {
    Node node = getNode(key);
    // 存在则更新
    if(node != null) {
      node.value = value;
      return;
    }
    node = new Node(key, value);
    if(keyToNodeMap.size() >= capacity) {
      // 删除链表尾部结点
      Node tail = dummy.prev;
      keyToNodeMap.remove(tail.key);
      remove(tail);
    }
    // 插入头节点
    keyToNodeMap.put(key, node);
    pushFront(node);

  }
  private Node getNode(int key) {
    if(!keyToNodeMap.containsKey(key)) {
      return null;
    }
    Node node = keyToNodeMap.get(key);
    remove(node);
    pushFront(node);
    return node;
  }
  private void remove(Node node) {
    node.prev.next = node.next;
    node.next.prev = node.prev;
  }
  private void pushFront(Node node) {
    node.next = dummy.next;
    node.next.prev = node;
    dummy.next = node;
    node.prev = dummy;
  }
}
```

# LFU

LRU是指淘汰策略是：淘汰很久没有被访问过的数据，以最近一次访问时间为参考。

最少使用策略LFU（Least Frequently Used）则是淘汰最近一段时间被访问次数最少的数据，跟LRU还是有区别的。

比如容量为3的缓存，插入key1, key2, key3，

1. 如果访问key2，然后插入key4时，key1则会被删掉，因为key1和key3访问次数都为1，但是key1插入时间更早。
2. 如果依次访问key1有10次，key2有8次，key3有6次，那么插入新的key4时，key3会被删除掉，即使它是最近访问的，如果是LRU，那么被干掉的就是key3了。

也就是说，LFU更适合热点数据的保留，体现在用户高频访问某些内容，但中间一小段时间未访问。



如何存储？用Map<访问次数，双向链表>存储，链表头部是最近访问的。这样可以快速得到访问几次的结点。

插入数据时：

1. 如果该数据在缓存中，则更新该结点访问次数和值。
2. 如果该数据未在缓存中：

1. 如果缓存已满，移除最少访问次数且在尾部的结点
2. 如果缓存未满，则直接插入 访问次数+1的链表中

## [460. LFU 缓存](https://leetcode.cn/problems/lfu-cache/)

```java
class LFUCache {
  public static class Node {
    private int key;
    private int value;
    // 访问频率
    private int freq; 
    Node prev;
    Node next;
    Node(int key, int value) {
      this.key = key;
      this.value = value;
      // 插入链表时进行+1
      this.freq = 0;
    }
  }
  private final int capacity;
  private final Map<Integer, Node> keyToNodeMap;
  private final Map<Integer, Node> freqToDummyMap; 
  private int minFreq;

  public LFUCache(int capacity) {
    this.capacity = capacity;
    keyToNodeMap = new HashMap<>();
    freqToDummyMap = new HashMap<>();
  }


  public int get(int key) {
    Node node = getNode(key);
    return node == null ? -1 : node.value;
  }
  /**
        1. 如果该数据未缓存中，返回null
        2. 如果该数据在缓存中，则将该结点访问次数+1，并移动到链表头部。
            2.1 旧的访问次数的链表如果为空，链表也需要删除；
            2.2 旧的访问次数如果是最小频次的，那么最小频次还需要进行更新。
     */
  private Node getNode(int key) {
    if(!keyToNodeMap.containsKey(key)) {
      return null;
    }
    Node node = keyToNodeMap.get(key);
    int freq = node.freq;

    remove(node);
    Node dummy = freqToDummyMap.get(freq);
    System.out.println("freq: " + freq + ": " + dummy.key);
    // 如果该频次链表为空后
    if(dummy.prev == dummy) {
      // 移除空链表
      freqToDummyMap.remove(freq);
      // 如果是最小频次的链表被移除了，最小频次+1
      if(minFreq == freq) {
        minFreq++;
      }
    }

    pushFront(freq + 1, node);
    return node;
  }
  private void remove(Node node) {
    node.prev.next = node.next;
    node.next.prev = node.prev;

  }
  private void pushFront(int freq, Node node) {
    Node dummy = freqToDummyMap.get(freq);
    if(dummy == null) {
      dummy = new Node(-1, -1);
      dummy.next = dummy;
      dummy.prev = dummy;
      freqToDummyMap.put(freq, dummy);
    }
    node.next = dummy.next;
    node.next.prev = node;
    dummy.next = node;
    node.prev = dummy;
    node.freq++;
  }

  /**
    最早插入的必然是在链表头部的 。这样可以快速得到访问几次的结点。
    插入数据时：
    1. 如果该数据在缓存中，则更新该结点访问次数和值。
    2. 如果该数据未在缓存中：
        a. 如果缓存已满，移除最少访问次数的且在尾部的结点
        b. 如果缓存未满，则直接插入 访问次数+1的链表中
    */
  public void put(int key, int value) {
    Node node = getNode(key);
    if(node != null) {
      node.value = value;
      return;
    }
    // 缓存已满
    if(keyToNodeMap.size() == capacity) {
      Node dummy = freqToDummyMap.get(minFreq);
      Node tail = dummy.prev;
      remove(tail);
      keyToNodeMap.remove(tail.key);
      // 链表为空
      if(dummy.prev == dummy) {
        freqToDummyMap.remove(minFreq);
      }
    }
    // 该数据未在缓存中
    node = new Node(key, value);
    keyToNodeMap.put(key, node);
    pushFront(1, node);
    minFreq = 1;
  }
}
```
