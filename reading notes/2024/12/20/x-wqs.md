cbc-mac
把AES当做底层分组随机函数。
获取消息并分解成块，每个块都与底层函数F一样长。运行通过CBC链，加密第一个块，并与第二个块异或，然后再将输入到F中。得到一个标签。标签将是n位长。这个标签可以截断处理，只要可以忽略。FECBC接受一对密钥作为输入，可以处理可变长的消息，生成一个标签。
没有最后加密步骤的函数称为原始CBC函数，也被称为原始密码分组链接，但其实不安全，所以需要最后一个函数，来确保安全。

nmac nested mac
cascade 级联函数，并不是一个安全的mac。需要，需要有一个fpad填充，确保密钥在X空间中，这样可以将输入到函数中了。最后需要有一个独立密钥作为F的输入。