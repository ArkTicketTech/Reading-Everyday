### word2vec
word2vec uses shallow neural networks that associate words to distributed representations.
It can capture many linguistic regularities.
Word2vec使用了滑动窗口构造训练数据。窗口里最中间的是target word，其他的是context word。
CBOW(Continuous Bag-of-Words): 给定了context word，预测出来target word。
skip-gram是通过target word预测context word。
sub-sampling 平衡常见词和罕见词。词频越高，权重越低。
soft sliding window 离target word更近的词，有更大的几率被采样成context word 进行训练。
