All Python objects that do not evaluate to a number are **ignored**.

All strings that cannot be evaluated to a number via C `strtod` function count as `0`'s.

Setting `only_str` to `True` will cause all non-strings in iterables to be **ignored**.

Setting `only_num` to `True` will cause all strings in iterables to be **ignored**.

### **sum**

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

### **max**

```python
numagr.max(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and finds the largest value.

**Return (success):**

- `float` max value
- `float` -inf (element count is zero)
- `float` +inf / -inf (overflow)

**Return (failure):**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **min**

```python
numagr.min(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and finds the smallest value.

**Return (success):**

- `float` max value
- `float` +inf (element count is zero)
- `float` +inf / -inf (overflow)

**Return (failure):**

- built-in Python exceptions

<p align="right">
<a href="#api">GO TO API ^</a>
</p>

---

### **amean**

```python
numagr.amean(*iterables, only_str = False, only_num = False)
```

Takes arbitrary iterables and calculates arithmetic mean.

**Return (success):**

- `float` arithmetic mean
- `float` inf (element count is `0`)

**Return (failure):**

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

### **hmean**

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

### **nearest**

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

### **count**

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

### **frac_below**

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

### **frac_above**

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

### **mode**

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

### **var**

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

### **pvar**

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

### **stdev**

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

### **pstdev**

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
