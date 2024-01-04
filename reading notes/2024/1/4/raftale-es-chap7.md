1. Overview of text analysis
2. Anatomy of an analyzer
3. Built-in analyzers
4. Developing custom analyzers
5. Understanding tokenizers
6. Learning about character and token filters



text analysis是指分词并存储的过程，这是通过一个叫做analyzers处理的。

默认的`analyzer`是`standard analyzer`.

# 7.1 Overview

数据存储和查询时都会进行`text analysis`，查询时一般都会使用相同analyzer，但也有时会使用另外的analyzer。



当我们搜索一个词的时候，如果输入的是同义词、缩写、首字母省略词、emoijs、各国语言等，也是希望能搜索出结果的。

analyzer有助于实现这一目标。

# Analyzer module

analyzer是一个模块，包含了两个功能：Tokenization和Normalization

## Tokenization

Tokenization, as the name indicates, is a process of splitting sentences into individual words by following certain rules.

这个过程由Tokenizer处理。

## Normalization

Normalization is where the tokens (words) are massaged, transformed, modified, and enriched in the form of stemming, synonyms, stop words, and other features.

1. stemming：stemming is an operation where the words are reduced (stemmed) to their root words. For example, “author” is a root word for “authors”, “authoring”, and “authored”
2. stop words：a, an, and, is, but, the, and so on。这些词对查找相关性毫无用处。

## Anatomy of an analyzer

Tokenization and normalization are carried out by essentially three software components:

1. character filters：作用到character上，过滤掉一些不必要的character，Elasticsearch提供了三种filter。

1. html_strip, mapping and pattern replace

1. tokenizers：split the text into individual words(aka, token) by using a delimiter such as whitespace, punctuation, or some form of word boundaries.

1. every analyzer must have one and only one tokenizer
2. 默认standard tokenizer，按照grammer和标点符号分割words

1. token filters：token filters is optional, they can be zero or many. The tokens then pass through the token filters where they get modified, enriched and enhanced.

1. change the case,
2. create synonyms
3. provide the root word(stemming)
4. produce n-grams and shingles



character filters和token filters是可选的，可以同时应用多个filter，但至少要有一个tokenizers。


