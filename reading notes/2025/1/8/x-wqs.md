资源访问设定：
有一个私有资源（放在private字段resource中），希望限制访问，只有在allowed users数组中的用户才能访问该资源，若不在此列表中尝试访问则抛出异常。
提供get allowed users方法返回string数组，用于展示允许访问的用户列表，方便判断当前用户是否有权限等。
use the resource方法会遍历allowed users数组，判断当前用户是否在其中，若在则允许按规定方式使用资源，若遍历完for循环未返回（即没找到匹配用户）则抛出异常。


安全漏洞分析：
get allowed users方法返回的是数组的别名。
恶意用户可调用get allowed users获取数组别名后，更新数组元素（比如将位置 0 的元素改为当前用户），这样再调用use the resource时，该用户就能访问资源了，即便原本不该有权限。
问题出在get allowed users方法泄露了allowed users数组的别名，原本假设客户端只会读取结果，但 Java 允许修改数组元素，导致出现安全漏洞。
