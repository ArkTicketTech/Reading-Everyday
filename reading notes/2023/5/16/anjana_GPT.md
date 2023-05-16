Transformer, the first sequence transduction model based entirely on attention, replacing the recurrent layers most commonly used in encoder-decoder architectures with multi-headed self-attention.

关键点：

sequence transduction model 序列转录模型 

encoder-decoder 

multi-headed self-attention



GPT-1论文：Improve Language Understanding by Generative Pre-Training

tasks: textual entailment, question answering, semantic similarity assessment, and document classification.

1. generative pre-training of a language model on a diverse corpus of unlabeled text
2. discriminative fine-tuning on each specific task

先训练好预训练模型，再进行微调。

task-aware input 任务相关的输入 during fine-tuning



GPT-2论文：Language Models are Unsupervised Multitask Learners

Conclusion: GPT-2 zero-shots to state of the art performance on 7 out of 8 tested language modeling datasets.

The diversity of tasks the model is able to perform in a zero-shot setting suggests that high-capacity models trained to maximize the likelihood of a sufficiently varied text corpus begin to learn how to perform a surprising amount of tasks without the need for explicit supervision.



GPT-3论文：Language Models are Few-Shot Learners

GPT-3, an autoregressive language model with 175 billion parameters, 10x more than any previous non-sparse model, and test its performance in the few-shot setting.

non-sparse 非稀疏

For all tasks, GPT-3 is applied without any gradient updates or fine-tuning, with tasks and few-shot demonstrations specified purely via text interaction with the model.

gradient updates 梯度更新

GPT-3论文中使用了”strong performance“这样的描述，的确是“大力出奇迹”了。也是因为有Microsoft金主爸爸🌚。

GPT-3 achieves strong performance on many NLP datasets, including translation, question-answering, and cloze tasks, as well as several tasks that require on-the-fly reasoning or domain adaptation, such as unscrambling words, using a novel word in a sentence, or performing 3-digit arithmetic.



GPT-3: No gradient updates are performed.

zero-shot: only a task description

one-shot: a task description and a single example

few-shot: a task description and a few examples