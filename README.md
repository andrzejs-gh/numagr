## Table of contents

- [Description](#description)
- [Usage](#usage)
- [API](#api)

## Description

**NUMAGR** is a Python C extension offering efficient statistical methods for numerical data. 

It's faster compared to standard and to other third party modules. It's also different in that it accepts and converts Python's numeric strings "on the fly" (using C `strtod` function), without having to initialize intermediate Python objects to hold the values. 

For sumations the module implements Kahan algorithm to retain precission.

<p align="right">
<a href="#table-of-contents">GO TO TABLE OF CONTENTS ^</a>
</p>

***

## Usage

Inside iterables, all strings that cannot be evaluated to a number via C `strtod` function count as `0`'s, and all Python objects that do not evaluate to a number are **ignored**.

Examples:
```python
numagr.sum([1,2,3,4,5,6,7,8,9])                           # 45

numagr.sum([1,2,"3",4,"5",6,"7",8,9])                     # 45

numagr.sum([None,1,"abc",2,"3",[],4,"5",6,"7",8,print,9]) # 45

numagr.amean([1,2,3,4])                 # 2.5

numagr.amean([1,2,3,4, "abc", None])    # 2.0 ( "abc" became 0, None was ignored )
```

Setting `only_str` to `True` will cause all non-strings in iterables to be **ignored**.

Setting `only_num` to `True` will cause all strings in iterables to be **ignored**.

```python
numagr.count(5, [1,2,5,"5","5","5",6,7], only_str = True) # 3

numagr.count(5, [1,2,5,"5","5","5",6,7], only_num = True) # 1
```

The kwargs are mutually exclusive, they cannot be both `True` at the same time.

```python
random_floats = [random.random() for _ in range(1_000_000)]
numagr.stdev(random_floats, only_str = True, only_num = True) # TypeError
# TypeError: Keyword arguments "str_only" and "num_only" are mutually exclusive.
```

<p align="right">
<a href="#table-of-contents">GO TO TABLE OF CONTENTS ^</a>
</p>

***

## API

- [sum](#sum)
- [max](#max)
- [min](#min)
- [amean](#amean)
- [gmean](#gmean)
- [hmean](#hmean)
- [nearest](#nearest)
- [count](#count)
- [frac_below](#frac_below)
- [frac_above](#frac_above)
- [mode](#mode)
- [var](#var)
- [pvar](#pvar)
- [stdev](#stdev)
- [pstdev](#pstdev)

***

### **sum**

```python
numagr.sum(*iterables, only_str = False, only_num = False)
```

Calculates the sum.


**Return:**

- `float` sum

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **max**

```python
numagr.max(*iterables, only_str = False, only_num = False)
```

Returns the largest value.

**Return:**

- `float` max value
- `float` -`inf` (if it's the largest value, if element count is zero, or if all elements are NaN(s))

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **min**

```python
numagr.min(*iterables, only_str = False, only_num = False)
```

Returns the smallest value.

**Return:**

- `float` max value
- `float` `inf` (if it's the smallest value, if element count is zero, or if all elements are NaN(s))

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **amean**

```python
numagr.amean(*iterables, only_str = False, only_num = False)
```

Calculates arithmetic mean.

**Return:**

- `float` arithmetic mean
- `float` nan (element count is `0`)

**Exceptions:**

- `numagr.CountOverflow` (element count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **gmean**

```python
numagr.gmean(*iterables, only_str = False, only_num = False)
```

Calculates geometric mean.

**Return:**

- `float` geometric mean
- `float` nan (element count is `0`)

**Exceptions:**

- `numagr.InvalidStringValue` (one of the iterables contain a string that cannot be evaluated to a value greater than `0`)
- `numagr.NegativeValue` (the iterable contains a negative value)
- `numagr.CountOverflow` (element count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **hmean**

```python
numagr.hmean(*iterables, only_str = False, only_num = False)
```

Calculates harmonic mean.

**Return:**

- `float` harmonic mean
- `float` nan (element count is `0`)

**Exceptions:**

- `numagr.NegativeValue` (one of the iterables contains a negative value)
- `numagr.ZeroValue` (one of the iterables contains a `0`)
- `numagr.CountOverflow` (element count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **nearest**

```python
numagr.nearest(target_value, *iterables, only_str = False, only_num = False)
```

Finds the value closest to a specified target value. The target value must be a finite number (`int` or `float`), passing `inf` or `nan` will raise `TypeError`.

**Return:**

- `float` nearest value
- `float` `inf` (no finite values in the iterables)

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **count**

```python
numagr.count(target_value, *iterables, only_str = False, only_num = False)
```

Returns the number of occurences of a specified target value. The target value must be a finite number (`int` or `float`), passing `inf` or `nan` will raise `TypeError`.

**Return:**

- `int` number of occurences

**Exceptions:**

- `numagr.CountOverflow` (occurences count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **frac_below**

```python
numagr.frac_below(target_value, *iterables, only_str = False, only_num = False)
```

Returns fraction of values that are below a specified target value. The target value must be a finite number (`int` or `float`), passing `inf` or `nan` will raise `TypeError`.

**Return:**

- `float` fraction of values below the target value

**Exceptions:**

- `numagr.CountOverflow` (element count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **frac_above**

```python
numagr.frac_above(target_value, *iterables, only_str = False, only_num = False)
```

Returns fraction of values that are above a specified target value. The target value must be a finite number (`int` or `float`), passing `inf` or `nan` will raise `TypeError`.

**Return:**

- `float` fraction of values above the target value

**Exceptions:**

- `numagr.CountOverflow` (element count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **mode**

```python
numagr.mode(*iterables, only_str = False, only_num = False)
```

Calculates the mode. Ignores any `nan`'s in the data. If the element count is `0`, the function returns `nan`.

**Return:**

- `float` mode

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **var**

```python
numagr.var(*iterables, only_str = False, only_num = False)
```

Calculates the variance. If there are any `inf`'s or `nan`'s in the data, or if the element count is `0` or `1`, the function returns `nan`.

**Return:**

- `float` variance

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **pvar**

```python
numagr.pvar(*iterables, only_str = False, only_num = False)
```

Calculates the population variance. If there are any `inf`'s or `nan`'s in the data, or if the element count is `0`, the function returns `nan`.

**Return:**

- `float` population variance

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **stdev**

```python
numagr.stdev(*iterables, only_str = False, only_num = False)
```

Calculates standard deviation. If there are any `inf`'s or `nan`'s in the data, or if the element count is `0` or `1`, the function returns `nan`.

**Return:**

- `float` standard deviation

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **pstdev**

```python
numagr.pstdev(*iterables, only_str = False, only_num = False)
```

Calculates population standard deviation. If there are any `inf`'s or `nan`'s in the data, or if the element count is `0`, the function returns `nan`.

**Return:**

- `float` population standard deviation

**Exceptions:**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---
