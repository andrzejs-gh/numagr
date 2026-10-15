All Python objects that do not evaluate to a number are **ignored**.

All strings that cannot be evaluated to a number via C `strtod` function count as `0`'s.

Setting `only_str` to `True` will cause all non-strings in iterables to be **ignored**.

Setting `only_num` to `True` will cause all strings in iterables to be **ignored**.

### **numagr.sum**

```python
numagr.sum(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and calculates the sum.


**Return (success):**

- `float` sum
- `float` +inf / -inf (overflow)

**Return (failure):**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.max**

```python
numagr.max(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and finds the largest value.

**Return (success):**

- `float` max value
- `float` +inf / -inf (overflow)

**Return (failure):**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.min**

```python
numagr.min(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and finds the smallest value.

**Return (success):**

- `float` min value
- `float` +inf / -inf (overflow)

**Return (failure):**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.amean**

```python
numagr.amean(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and calculates arithmetic mean.

**Return (success):**

- `float` arithmetic mean
- `float` inf (element count is `0`)

**Return (failure):**

- `RuntimeError` (element count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.gmean**

```python
numagr.gmean(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and calculates geometric mean.

**Return (success):**

- `float` geometric mean
- `float` inf (element count is `0`)

**Return (failure):**

- `RuntimeError` (one of the iterables contain a string that cannot be evaluated to a value greater than `0`)
- `RuntimeError` (element count exceeds unsigned 64-bit integer capacity)
- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.hmean**

```python
numagr.hmean(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and calculates harmonic mean.

**Return (success):**

- `float` harmonic mean
- `float` inf (element count is `0`)

**Return (failure):**

- `RuntimeError` (one of the iterables contains a `0`)

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.nearest**

```python
numagr.nearest(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.count**

```python
numagr.count(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.frac_below**

```python
numagr.frac_below(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.frac_above**

```python
numagr.frac_above(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.mode**

```python
numagr.mode(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.var**

```python
numagr.var(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.pvar**

```python
numagr.pvar(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.stdev**

```python
numagr.stdev(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **numagr.pstdev**

```python
numagr.pstdev(*iterables, only_str = False, only_num = False)
```

**Return (success):**

- Result

**Return (failure):**

- Error

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---
