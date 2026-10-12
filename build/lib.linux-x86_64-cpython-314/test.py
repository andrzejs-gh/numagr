import numagr
import math
from random import randint, random, choice
from collections import Counter
import time
import numpy
import statistics
import sys

LIMIT = 1_000_000
ITERATIONS = 1

FLOATS = []
INTS = []
MIXED_SAFE = []
MIXED_HAZ = []

def RED(txt):
	return f"\033[31m{txt}\033[0m"

def GREEN(txt):
	return f"\033[32m{txt}\033[0m"

def LIGHT_GREEN(txt):
	return f"\033[92m{txt}\033[0m"

def BLUE(txt):
	return f"\033[34m{txt}\033[0m"

def LIGHT_BLUE(txt):
	return f"\033[94m{txt}\033[0m"

def CYAN(txt):
	return f"\033[36m{txt}\033[0m"

def MAGENTA(txt):
	return f"\033[95m{txt}\033[0m"

def YELLOW(txt):
	return f"\033[33m{txt}\033[0m"

def LIGHT_YELLOW(txt):
	return f"\033[93m{txt}\033[0m"

OK = f"[ {GREEN('OK')} ]"
ERR = f"[ {RED('OK')} ]"

def NEW_LINE():
	print("\n")

def load_lists():

	print("Initializing iterables...", end = "", flush = True)

	non_numables = ((1,2,3), [1,2,3], b"bytes", load_lists, numagr,
					"some string", {1, None, True}, None)
	sign = (-1, 1)

	global FLOATS, INTS, MIXED_SAFE, MIXED_HAZ

	FLOATS = [random() + 1 for _ in range(LIMIT)]
	INTS = [randint(1, LIMIT) for _ in range(LIMIT)]

	for i in range(1, LIMIT+1):

		r = randint(1, 6)

		match r:
			case 1:
				MIXED_SAFE.append(i)
			case 2:
				MIXED_SAFE.append(str(i))
			case 3:
				MIXED_SAFE.append(random()+1.0)
			case 4:
				MIXED_SAFE.append(str(random()+1.0))
			case 5:
				MIXED_SAFE.append(randint(1, LIMIT))
			case 6:
				MIXED_SAFE.append(str(randint(1, LIMIT)))

	for i in range(LIMIT):

		r = randint(0, 6)

		match r:
			case 0:
				MIXED_HAZ.append(i*choice(sign))
			case 1:
				MIXED_HAZ.append(str(i*choice(sign)))
			case 2:
				MIXED_HAZ.append(random()*choice(sign))
			case 3:
				MIXED_HAZ.append(str(random()*choice(sign)))
			case 4:
				MIXED_HAZ.append(randint(1, LIMIT)*choice(sign))
			case 5:
				MIXED_HAZ.append(str(randint(1, LIMIT)*choice(sign)))
			case 6:
				MIXED_HAZ.append(choice(non_numables))

	print(OK)

def get_xtime(method, *args, **kwargs):

	t1 = time.perf_counter()
	ret = method(*args, **kwargs)
	t2 = time.perf_counter()

	return (ret, t2 - t1)

def benchmark(method, *args, **kwargs):

	t = 0.0
	for _ in range(ITERATIONS):
		ret, xt = get_xtime(method, *args, **kwargs)
		t += xt

	return  (ret, t / ITERATIONS)

def normalize_mixed(mixed_list, ph_value = 0.0, only_nums = False):

	if only_nums:
		return [x for x in mixed_list if isinstance(x, (int, float))]
	else:
		normalized_list = []
		for x in mixed_list:
			try:
				normalized_list.append(float(x))
			except (ValueError, TypeError):
				normalized_list.append(ph_value)

		return normalized_list

def std_variance(iterable, count):

		 mean = sum(iterable) / count
		 return sum((x - mean)**2 for x in iterable) / count

def sum_test():

	print(YELLOW("Sum benchmark..."))

	sum_ints, sum_ints_time = benchmark(sum, INTS)
	math_fsum_ints, math_fsum_ints_time = benchmark(math.fsum, INTS)
	numpy_sum_ints, numpy_sum_ints_time = benchmark(numpy.sum, INTS)
	numagr_sum_ints, numagr_sum_ints_time = benchmark(numagr.sum, INTS)

	print(LIGHT_BLUE("Summing integers:"))

	print(f"{LIGHT_YELLOW('sum       ')} t = {sum_ints_time}\n{sum_ints}")
	print(f"{LIGHT_YELLOW('math.fsum ')} t = {math_fsum_ints_time}\n{math_fsum_ints}")
	print(f"{LIGHT_YELLOW('numpy.sum ')} t = {numpy_sum_ints_time}\n{numpy_sum_ints}")
	print(f"{LIGHT_GREEN( 'numagr.sum')} t = {numagr_sum_ints_time} {LIGHT_GREEN('<-')}\n{numagr_sum_ints}")

	print(LIGHT_BLUE("Summing floats:"))

	sum_floats, sum_floats_time = benchmark(sum, FLOATS)
	math_fsum_floats, math_fsum_floats_time = benchmark(math.fsum, FLOATS)
	numpy_sum_floats, numpy_sum_floats_time = benchmark(numpy.sum, FLOATS)
	numagr_sum_floats, numagr_sum_floats_time = benchmark(numagr.sum, FLOATS)

	print(f"{LIGHT_YELLOW('sum       ')} t = {sum_floats_time}\n{sum_floats}")
	print(f"{LIGHT_YELLOW('math.fsum ')} t = {math_fsum_floats_time}\n{math_fsum_floats}")
	print(f"{LIGHT_YELLOW('numpy.sum ')} t = {numpy_sum_floats_time}\n{numpy_sum_floats}")
	print(f"{LIGHT_GREEN( 'numagr.sum')} t = {numagr_sum_floats_time} {LIGHT_GREEN('<-')}\n{numagr_sum_floats}")

	# get normalized MIXED_SAFE============================================
	normalized_MIXED, normal_time = get_xtime(normalize_mixed, MIXED_SAFE)
	# =====================================================================
	sum_mixed, sum_mixed_time = benchmark(sum, normalized_MIXED)
	math_fsum_mixed, math_fsum_mixed_time = benchmark(math.fsum,
													  normalized_MIXED)
	numpy_sum_mixed, numpy_sum_mixed_time = benchmark(numpy.sum,
													  normalized_MIXED)
	numagr_sum_mixed, numagr_sum_mixed_time = benchmark(numagr.sum, MIXED_SAFE)

	print(LIGHT_BLUE("Summing mixed values:"))

	print(f"{LIGHT_YELLOW('sum       ')} t = {sum_mixed_time + normal_time}\n{sum_mixed}")
	print(f"{LIGHT_YELLOW('math.fsum ')} t = {math_fsum_mixed_time + normal_time}\n{math_fsum_mixed}")
	print(f"{LIGHT_YELLOW('numpy.sum ')} t = {numpy_sum_mixed_time + normal_time}\n{numpy_sum_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.sum')} t = {numagr_sum_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_sum_mixed}")

	print("Sum benchmark completed", OK)

def min_test():

	print(YELLOW("Min benchmark..."))

	min_ints, min_ints_time = benchmark(min, INTS)
	numpy_min_ints, numpy_min_ints_time = benchmark(numpy.min, INTS)
	numagr_min_ints, numagr_min_ints_time = benchmark(numagr.min, INTS)

	print(LIGHT_BLUE("Finding minimum in integers:"))

	print(f"{LIGHT_YELLOW('min       ')} t = {min_ints_time}\n{min_ints}")
	print(f"{LIGHT_YELLOW('numpy.min ')} t = {numpy_min_ints_time}\n{numpy_min_ints}")
	print(f"{LIGHT_GREEN( 'numagr.min')} t = {numagr_min_ints_time} {LIGHT_GREEN('<-')}\n{numagr_min_ints}")

	print(LIGHT_BLUE("Finding minimum in floats:"))

	min_floats, min_floats_time = benchmark(min, FLOATS)
	numpy_min_floats, numpy_min_floats_time = benchmark(numpy.min, FLOATS)
	numagr_min_floats, numagr_min_floats_time = benchmark(numagr.min, FLOATS)

	print(f"{LIGHT_YELLOW('min       ')} t = {min_floats_time}\n{min_floats}")
	print(f"{LIGHT_YELLOW('numpy.min ')} t = {numpy_min_floats_time}\n{numpy_min_floats}")
	print(f"{LIGHT_GREEN( 'numagr.min')} t = {numagr_min_floats_time} {LIGHT_GREEN('<-')}\n{numagr_min_floats}")

	# get normalized MIXED_HAZ=============================================
	normalized_MIXED, normal_time = get_xtime(normalize_mixed, MIXED_HAZ)
	# =====================================================================
	min_mixed, min_mixed_time = benchmark(min, normalized_MIXED)
	numpy_min_mixed, numpy_min_mixed_time = benchmark(numpy.min,
													normalized_MIXED)
	numagr_min_mixed, numagr_min_mixed_time = benchmark(numagr.min, MIXED_HAZ)

	print(LIGHT_BLUE("Finding minimum of mixed values:"))

	print(f"{LIGHT_YELLOW('min       ')} t = {min_mixed_time + normal_time}\n{min_mixed}")
	print(f"{LIGHT_YELLOW('numpy.min ')} t = {numpy_min_mixed_time + normal_time}\n{numpy_min_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.min')} t = {numagr_min_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_min_mixed}")

	print("Min benchmark completed", OK)

def max_test():

	print(YELLOW("Max benchmark..."))

	max_ints, max_ints_time = benchmark(max, INTS)
	numpy_max_ints, numpy_max_ints_time = benchmark(numpy.max, INTS)
	numagr_max_ints, numagr_max_ints_time = benchmark(numagr.max, INTS)

	print(LIGHT_BLUE("Finding maximum in integers:"))

	print(f"{LIGHT_YELLOW('max       ')} t = {max_ints_time}\n{max_ints}")
	print(f"{LIGHT_YELLOW('numpy.max ')} t = {numpy_max_ints_time}\n{numpy_max_ints}")
	print(f"{LIGHT_GREEN( 'numagr.max')} t = {numagr_max_ints_time} {LIGHT_GREEN('<-')}\n{numagr_max_ints}")

	print(LIGHT_BLUE("Finding maximum in floats:"))

	max_floats, max_floats_time = benchmark(max, FLOATS)
	numpy_max_floats, numpy_max_floats_time = benchmark(numpy.max, FLOATS)
	numagr_max_floats, numagr_max_floats_time = benchmark(numagr.max, FLOATS)

	print(f"{LIGHT_YELLOW('max       ')} t = {max_floats_time}\n{max_floats}")
	print(f"{LIGHT_YELLOW('numpy.max ')} t = {numpy_max_floats_time}\n{numpy_max_floats}")
	print(f"{LIGHT_GREEN( 'numagr.max')} t = {numagr_max_floats_time} {LIGHT_GREEN('<-')}\n{numagr_max_floats}")

	# get normalized MIXED_SAFE============================================
	normalized_MIXED, normal_time = get_xtime(normalize_mixed, MIXED_SAFE)
	# =====================================================================
	max_mixed, max_mixed_time = benchmark(max, normalized_MIXED)
	numpy_max_mixed, numpy_max_mixed_time = benchmark(numpy.max,
													  normalized_MIXED)
	numagr_max_mixed, numagr_max_mixed_time = benchmark(numagr.max, MIXED_SAFE)

	print(LIGHT_BLUE("Finding maximum of mixed values:"))

	print(f"{LIGHT_YELLOW('max       ')} t = {max_mixed_time + normal_time}\n{max_mixed}")
	print(f"{LIGHT_YELLOW('numpy.max ')} t = {numpy_max_mixed_time + normal_time}\n{numpy_max_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.max')} t = {numagr_max_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_max_mixed}")

	print("Max benchmark completed", OK)

def amean_test():

	print(YELLOW("Arithmetic mean benchmark..."))

	# defining std mean =========================================
	std_mean = lambda x: (sum(x) / len(x))
	# ===========================================================

	std_mean_ints, std_mean_ints_time = benchmark(std_mean, INTS)
	stat_mean_ints, stat_mean_ints_time = benchmark(statistics.mean, INTS)
	numpy_mean_ints, numpy_mean_ints_time = benchmark(numpy.mean, INTS)
	numagr_mean_ints, numagr_mean_ints_time = benchmark(numagr.amean, INTS)

	print(LIGHT_BLUE("Finding arithmetic mean in integers:"))

	print(f"{LIGHT_YELLOW('sum(x) / len(x)')} t = {std_mean_ints_time}\n{std_mean_ints}")
	print(f"{LIGHT_YELLOW('statistics.mean')} t = {stat_mean_ints_time}\n{stat_mean_ints}")
	print(f"{LIGHT_YELLOW('numpy.mean     ')} t = {numpy_mean_ints_time}\n{numpy_mean_ints}")
	print(f"{LIGHT_GREEN( 'numagr.amean   ')} t = {numagr_mean_ints_time} {LIGHT_GREEN('<-')}\n{numagr_mean_ints}")

	print(LIGHT_BLUE("Finding arithmetic mean in floats:"))

	std_mean_floats, std_mean_floats_time = benchmark(std_mean, FLOATS)
	stat_mean_floats, stat_mean_floats_time = benchmark(statistics.mean, FLOATS)
	numpy_mean_floats, numpy_mean_floats_time = benchmark(numpy.mean, FLOATS)
	numagr_mean_floats, numagr_mean_floats_time = benchmark(numagr.amean, FLOATS)

	print(f"{LIGHT_YELLOW('sum(x) / len(x)')} t = {std_mean_floats_time}\n{std_mean_floats}")
	print(f"{LIGHT_YELLOW('statistics.mean')} t = {stat_mean_floats_time}\n{stat_mean_floats}")
	print(f"{LIGHT_YELLOW('numpy.mean     ')} t = {numpy_mean_floats_time}\n{numpy_mean_floats}")
	print(f"{LIGHT_GREEN( 'numagr.amean   ')} t = {numagr_mean_floats_time} {LIGHT_GREEN('<-')}\n{numagr_mean_floats}")

	# get normalized MIXED_SAFE============================================
	normalized_MIXED, normal_time = get_xtime(normalize_mixed, MIXED_SAFE)
	# =====================================================================
	std_mean_mixed, std_mean_mixed_time = benchmark(std_mean, normalized_MIXED)
	stat_mean_mixed, stat_mean_mixed_time = benchmark(statistics.mean, normalized_MIXED)
	numpy_mean_mixed, numpy_mean_mixed_time = benchmark(numpy.mean,
														normalized_MIXED)
	numagr_mean_mixed, numagr_mean_mixed_time = benchmark(numagr.amean, MIXED_SAFE)

	print(LIGHT_BLUE("Finding arithmetic mean of mixed values:"))

	print(f"{LIGHT_YELLOW('sum(x) / len(x)')} t = {std_mean_mixed_time + normal_time}\n{std_mean_mixed}")
	print(f"{LIGHT_YELLOW('statistics.mean')} t = {stat_mean_mixed_time + normal_time}\n{stat_mean_mixed}")
	print(f"{LIGHT_YELLOW('numpy.mean     ')} t = {numpy_mean_mixed_time + normal_time}\n{numpy_mean_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.amean   ')} t = {numagr_mean_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_mean_mixed}")

	print("Arithmetic mean benchmark completed", OK)

def gmean_test():

	print(YELLOW("Geometric mean benchmark..."))

	stat_gmean_ints, stat_gmean_ints_time = benchmark(statistics.geometric_mean, INTS)
	numpy_gmean_ints, numpy_gmean_ints_time = benchmark(
		lambda x: numpy.exp(numpy.mean(numpy.log(x))), INTS)
	numagr_gmean_ints, numagr_gmean_ints_time = benchmark(numagr.gmean, INTS)

	print(LIGHT_BLUE("Finding geometric mean in integers:"))

	print(f"{LIGHT_YELLOW('statistics.geometric_mean')} t = {stat_gmean_ints_time}\n{stat_gmean_ints}")
	print(f"{LIGHT_YELLOW('numpy.exp(mean(log(x)))  ')} t = {numpy_gmean_ints_time}\n{numpy_gmean_ints}")
	print(f"{LIGHT_GREEN( 'numagr.gmean             ')} t = {numagr_gmean_ints_time} {LIGHT_GREEN('<-')}\n{numagr_gmean_ints}")

	print(LIGHT_BLUE("Finding geometric mean in floats:"))

	stat_gmean_floats, stat_gmean_floats_time = benchmark(statistics.geometric_mean, FLOATS)
	numpy_gmean_floats, numpy_gmean_floats_time = benchmark(
		lambda x: numpy.exp(numpy.mean(numpy.log(x))), FLOATS)
	numagr_gmean_floats, numagr_gmean_floats_time = benchmark(numagr.gmean, FLOATS)

	print(f"{LIGHT_YELLOW('statistics.geometric_mean')} t = {stat_gmean_floats_time}\n{stat_gmean_floats}")
	print(f"{LIGHT_YELLOW('numpy.exp(mean(log(x)))  ')} t = {numpy_gmean_floats_time}\n{numpy_gmean_floats}")
	print(f"{LIGHT_GREEN( 'numagr.gmean             ')} t = {numagr_gmean_floats_time} {LIGHT_GREEN('<-')}\n{numagr_gmean_floats}")

	# get normalized MIXED_SAFE============================================
	normalized_MIXED, normal_time = get_xtime(normalize_mixed, MIXED_SAFE,
										      only_nums = True)
	# =====================================================================

	stat_gmean_mixed, stat_gmean_mixed_time = benchmark(
		statistics.geometric_mean, normalized_MIXED)
	numpy_gmean_mixed, numpy_gmean_mixed_time = benchmark(
		lambda x: numpy.exp(numpy.mean(numpy.log(x))), normalized_MIXED)
	numagr_gmean_mixed, numagr_gmean_mixed_time = benchmark(numagr.gmean,
														    MIXED_SAFE, only_num=True)

	print(LIGHT_BLUE("Finding geometric mean of mixed values:"))

	print(f"{LIGHT_YELLOW('statistics.geometric_mean')} t = {stat_gmean_mixed_time + normal_time}\n{stat_gmean_mixed}")
	print(f"{LIGHT_YELLOW('numpy.exp(mean(log(x)))  ')} t = {numpy_gmean_mixed_time + normal_time}\n{numpy_gmean_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.gmean             ')} t = {numagr_gmean_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_gmean_mixed}")

	print("Geometric mean benchmark completed", OK)

def hmean_test():

	print(YELLOW("Harmonic mean benchmark..."))

	# defining std harmonic mean =================================
	std_hmean = lambda X: len(X) / sum(1 / value for value in X)
	# ============================================================
	# defining numpy harmonic mean =================================
	numpy_hmean = lambda X: len(X) / numpy.sum(1 / numpy.array(X))
	# ============================================================

	std_hmean_ints, std_hmean_ints_time = benchmark(std_hmean, INTS)
	stat_hmean_ints, stat_hmean_ints_time = benchmark(statistics.harmonic_mean, INTS)
	numpy_hmean_ints, numpy_hmean_ints_time = benchmark(numpy_hmean, INTS)
	numagr_hmean_ints, numagr_hmean_ints_time = benchmark(numagr.hmean, INTS)

	print(LIGHT_BLUE("Finding harmonic mean in integers:"))

	print(f"{LIGHT_YELLOW('len(x) / sum(1/x)        ')} t = {std_hmean_ints_time}\n{std_hmean_ints}")
	print(f"{LIGHT_YELLOW('statistics.harmonic_mean ')} t = {stat_hmean_ints_time}\n{stat_hmean_ints}")
	print(f"{LIGHT_YELLOW('len(x) / numpy.sum(1 / x)')} t = {numpy_hmean_ints_time}\n{numpy_hmean_ints}")
	print(f"{LIGHT_GREEN( 'numagr.hmean             ')} t = {numagr_hmean_ints_time} {LIGHT_GREEN('<-')}\n{numagr_hmean_ints}")

	print(LIGHT_BLUE("Finding harmonic mean in floats:"))

	std_hmean_floats, std_hmean_floats_time = benchmark(std_hmean, FLOATS)
	stat_hmean_floats, stat_hmean_floats_time = benchmark(statistics.harmonic_mean, FLOATS)
	numpy_hmean_floats, numpy_hmean_floats_time = benchmark(numpy_hmean, FLOATS)
	numagr_hmean_floats, numagr_hmean_floats_time = benchmark(numagr.hmean, FLOATS)

	print(f"{LIGHT_YELLOW('len(x) / sum(1/x)        ')} t = {std_hmean_floats_time}\n{std_hmean_floats}")
	print(f"{LIGHT_YELLOW('statistics.harmonic_mean ')} t = {stat_hmean_floats_time}\n{stat_hmean_floats}")
	print(f"{LIGHT_YELLOW('len(x) / numpy.sum(1 / x)')} t = {numpy_hmean_floats_time}\n{numpy_hmean_floats}")
	print(f"{LIGHT_GREEN( 'numagr.hmean             ')} t = {numagr_hmean_floats_time} {LIGHT_GREEN('<-')}\n{numagr_hmean_floats}")

	# get normalized MIXED_SAFE============================================
	normalized_MIXED, normal_time = get_xtime(normalize_mixed, MIXED_SAFE,
										      only_nums=True)
	# =====================================================================
	std_hmean_mixed, std_hmean_mixed_time = benchmark(std_hmean, normalized_MIXED)
	stat_hmean_mixed, stat_hmean_mixed_time = benchmark(
		statistics.harmonic_mean, normalized_MIXED)
	numpy_hmean_mixed, numpy_hmean_mixed_time = benchmark(numpy_hmean, normalized_MIXED)
	numagr_hmean_mixed, numagr_hmean_mixed_time = benchmark(
		numagr.hmean, MIXED_SAFE, only_num=True)

	print(LIGHT_BLUE("Finding harmonic mean of mixed values:"))

	print(f"{LIGHT_YELLOW('len(x) / sum(1/x)        ')} t = {std_hmean_mixed_time + normal_time}\n{std_hmean_mixed}")
	print(f"{LIGHT_YELLOW('statistics.harmonic_mean ')} t = {stat_hmean_mixed_time + normal_time}\n{stat_hmean_mixed}")
	print(f"{LIGHT_YELLOW('len(x) / numpy.sum(1 / x)')} t = {numpy_hmean_mixed_time + normal_time}\n{numpy_hmean_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.hmean             ')} t = {numagr_hmean_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_hmean_mixed}")

	print("Harmonic mean benchmark completed", OK)

def nearest_test():

	print(YELLOW("Nearest value benchmark..."))

	# defining std nearest =================================================
	std_nearest = lambda target, X: min(X, key = lambda v: abs(v - target))
	# ======================================================================

	target = randint(1, LIMIT)

	std_nearest_ints, std_nearest_ints_time = benchmark(std_nearest,
													    target, INTS)
	numagr_nearest_ints, numagr_nearest_ints_time = benchmark(numagr.nearest,
																target, INTS)

	print(LIGHT_BLUE(f"Finding nearest value in integers, target = {target}:"))

	print(f"{LIGHT_YELLOW('min(key=abs(x-target))')} t = {std_nearest_ints_time}\n{std_nearest_ints}")
	print(f"{LIGHT_GREEN( 'numagr.nearest        ')} t = {numagr_nearest_ints_time} {LIGHT_GREEN('<-')}\n{numagr_nearest_ints}")


	target = random()

	std_nearest_floats, std_nearest_floats_time = benchmark(std_nearest,
														    target, FLOATS)
	numagr_nearest_floats, numagr_nearest_floats_time = benchmark(numagr.nearest,
																  target, FLOATS)

	print(LIGHT_BLUE(f"Finding nearest value in floats, target = {target}:"))

	print(f"{LIGHT_YELLOW('min(key=abs(x-target))')} t = {std_nearest_floats_time}\n{std_nearest_floats}")
	print(f"{LIGHT_GREEN( 'numagr.nearest        ')} t = {numagr_nearest_floats_time} {LIGHT_GREEN('<-')}\n{numagr_nearest_floats}")

	target = random()*randint(1, LIMIT)*choice((-1, 1))

	# get normalized MIXED_HAZ============================================
	normalized_MIXED, normal_time = get_xtime(normalize_mixed, MIXED_HAZ,
										      only_nums=True)
	# =====================================================================

	std_nearest_mixed, std_nearest_mixed_time = benchmark(std_nearest, target,
													      normalized_MIXED)
	numagr_nearest_mixed, numagr_nearest_mixed_time = benchmark(numagr.nearest,
													target, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE(f"Finding nearest value of mixed values, target = {target}:"))

	print(f"{LIGHT_YELLOW('min(key=abs(x-target)')} t = {std_nearest_mixed_time + normal_time}\n{std_nearest_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.nearest       ')} t = {numagr_nearest_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_nearest_mixed}")

	print("Nearest value benchmark completed", OK)

def count_test():

	print(YELLOW("Count occurrences benchmark..."))

	# defining std count ==========================================
	std_count = lambda target, X: X.count(target)
	# =============================================================
	# defining numpy count ========================================
	numpy_count = lambda target, X: numpy.count_nonzero(numpy.array(X) == target)
	# =============================================================

	target = choice(INTS)

	std_count_ints, std_count_ints_time = benchmark(std_count, target, INTS)
	numpy_count_ints, numpy_count_ints_time = benchmark(numpy_count, target, INTS)
	numagr_count_ints, numagr_count_ints_time = benchmark(numagr.count, target, INTS)

	print(LIGHT_BLUE(f"Counting occurrences in integers, target = {target}:"))

	print(f"{LIGHT_YELLOW('x.count(target)    ')} t = {std_count_ints_time}\n{std_count_ints}")
	print(f"{LIGHT_YELLOW('numpy.count_nonzero')} t = {numpy_count_ints_time}\n{numpy_count_ints}")
	print(f"{LIGHT_GREEN( 'numagr.count       ')} t = {numagr_count_ints_time} {LIGHT_GREEN('<-')}\n{numagr_count_ints}")

	target = choice(FLOATS)

	std_count_floats, std_count_floats_time = benchmark(std_count, target, FLOATS)
	numpy_count_floats, numpy_count_floats_time = benchmark(numpy_count,
															target, FLOATS)
	numagr_count_floats, numagr_count_floats_time = benchmark(numagr.count,
															  target, FLOATS)

	print(LIGHT_BLUE(f"Counting occurrences in floats, target = {target}:"))

	print(f"{LIGHT_YELLOW('x.count(target)    ')} t = {std_count_floats_time}\n{std_count_floats}")
	print(f"{LIGHT_YELLOW('numpy.count_nonzero')} t = {numpy_count_floats_time}\n{numpy_count_floats}")
	print(f"{LIGHT_GREEN( 'numagr.count       ')} t = {numagr_count_floats_time} {LIGHT_GREEN('<-')}\n{numagr_count_floats}")

	# get normalized MIXED_HAZ============================================
	normalized_MIXED, normal_time = get_xtime(
								normalize_mixed, MIXED_HAZ, only_nums=True)
	# =====================================================================

	target = choice(normalized_MIXED)

	std_count_mixed, std_count_mixed_time = benchmark(std_count, target,
													  normalized_MIXED)
	numpy_count_mixed, numpy_count_mixed_time = benchmark(numpy_count,
														  target, normalized_MIXED)
	numagr_count_mixed, numagr_count_mixed_time = benchmark(numagr.count, target,
														    MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE(f"Counting occurrences of mixed values, target = {target}:"))

	print(f"{LIGHT_YELLOW('x.count(target)    ')} t = {std_count_mixed_time + normal_time}\n{std_count_mixed}")
	print(f"{LIGHT_YELLOW('numpy.count_nonzero')} t = {numpy_count_mixed_time + normal_time}\n{numpy_count_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.count       ')} t = {numagr_count_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_count_mixed}")

	print("Count occurrences benchmark completed", OK)

def fraction_below_test():

	print(YELLOW("Fraction below target benchmark..."))

	# defining std fraction ============================================
	std_fraction = lambda target, X: sum(v < target for v in X) / len(X)
	numpy_fraction = lambda target, X: numpy.mean(numpy.array(X) < target)
	# ==================================================================

	target = choice(INTS)

	std_fraction_ints, std_fraction_ints_time = benchmark(
		std_fraction, target, INTS)
	numpy_fraction_ints, numpy_fraction_ints_time = benchmark(
		numpy_fraction, target, INTS)
	numagr_fraction_ints, numagr_fraction_ints_time = benchmark(
		numagr.frac_below, target, INTS)

	print(LIGHT_BLUE(f"Finding fraction below target in integers, target = {target}:"))

	print(f"{LIGHT_YELLOW('sum(x < target) / len(x)')} t = {std_fraction_ints_time}\n{std_fraction_ints}")
	print(f"{LIGHT_YELLOW('numpy.mean(x < target)  ')} t = {numpy_fraction_ints_time}\n{numpy_fraction_ints}")
	print(f"{LIGHT_GREEN( 'numagr.frac_below       ')} t = {numagr_fraction_ints_time} {LIGHT_GREEN('<-')}\n{numagr_fraction_ints}")

	target = choice(FLOATS)

	print(LIGHT_BLUE(f"Finding fraction below target in floats, target = {target}:"))

	std_fraction_floats, std_fraction_floats_time = benchmark(
		std_fraction, target, FLOATS)
	numpy_fraction_floats, numpy_fraction_floats_time = benchmark(
		numpy_fraction, target, FLOATS)
	numagr_fraction_floats, numagr_fraction_floats_time = benchmark(
		numagr.frac_below, target, FLOATS)

	print(f"{LIGHT_YELLOW('sum(x < target) / len(x)')} t = {std_fraction_floats_time}\n{std_fraction_floats}")
	print(f"{LIGHT_YELLOW('numpy.mean(x < target)  ')} t = {numpy_fraction_floats_time}\n{numpy_fraction_floats}")
	print(f"{LIGHT_GREEN( 'numagr.frac_below       ')} t = {numagr_fraction_floats_time} {LIGHT_GREEN('<-')}\n{numagr_fraction_floats}")

	# get normalized MIXED_HAZ==========================================
	normalized_MIXED, normal_time = get_xtime(
		normalize_mixed, MIXED_HAZ, only_nums=True)
	# ==================================================================

	target = choice(normalized_MIXED)

	std_fraction_mixed, std_fraction_mixed_time = benchmark(
		std_fraction, target, normalized_MIXED)
	numpy_fraction_mixed, numpy_fraction_mixed_time = benchmark(
		numpy_fraction, target, normalized_MIXED)
	numagr_fraction_mixed, numagr_fraction_mixed_time = benchmark(
		numagr.frac_below, target, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE(
		f"Finding fraction below target in mixed values, target = {target}:"))

	print(f"{LIGHT_YELLOW('sum(x < target) / len(x)')} t = {std_fraction_mixed_time + normal_time}\n{std_fraction_mixed}")
	print(f"{LIGHT_YELLOW('numpy.mean(x < target)  ')} t = {numpy_fraction_mixed_time + normal_time}\n{numpy_fraction_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.frac_below       ')} t = {numagr_fraction_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_fraction_mixed}")

	print("Fraction below target benchmark completed", OK)

def fraction_above_test():

	print(YELLOW("Fraction above target benchmark..."))

	# defining std fraction ============================================
	std_fraction = lambda target, X: sum(v > target for v in X) / len(X)
	numpy_fraction = lambda target, X: numpy.mean(numpy.array(X) > target)
	# ==================================================================

	target = choice(INTS)

	std_fraction_ints, std_fraction_ints_time = benchmark(
		std_fraction, target, INTS)
	numpy_fraction_ints, numpy_fraction_ints_time = benchmark(
		numpy_fraction, target, INTS)
	numagr_fraction_ints, numagr_fraction_ints_time = benchmark(
		numagr.frac_above, target, INTS)

	print(LIGHT_BLUE(f"Finding fraction above target in integers, target = {target}:"))

	print(f"{LIGHT_YELLOW('sum(x > target) / len(x)')} t = {std_fraction_ints_time}\n{std_fraction_ints}")
	print(f"{LIGHT_YELLOW('numpy.mean(x > target)  ')} t = {numpy_fraction_ints_time}\n{numpy_fraction_ints}")
	print(f"{LIGHT_GREEN( 'numagr.frac_above       ')} t = {numagr_fraction_ints_time} {LIGHT_GREEN('<-')}\n{numagr_fraction_ints}")

	target = choice(FLOATS)

	print(LIGHT_BLUE(f"Finding fraction above target in floats, target = {target}:"))

	std_fraction_floats, std_fraction_floats_time = benchmark(
		std_fraction, target, FLOATS)
	numpy_fraction_floats, numpy_fraction_floats_time = benchmark(
		numpy_fraction, target, FLOATS)
	numagr_fraction_floats, numagr_fraction_floats_time = benchmark(
		numagr.frac_above, target, FLOATS)

	print(f"{LIGHT_YELLOW('sum(x > target) / len(x)')} t = {std_fraction_floats_time}\n{std_fraction_floats}")
	print(f"{LIGHT_YELLOW('numpy.mean(x > target)  ')} t = {numpy_fraction_floats_time}\n{numpy_fraction_floats}")
	print(f"{LIGHT_GREEN( 'numagr.frac_above       ')} t = {numagr_fraction_floats_time} {LIGHT_GREEN('<-')}\n{numagr_fraction_floats}")

	# get normalized MIXED_HAZ==========================================
	normalized_MIXED, normal_time = get_xtime(
		normalize_mixed, MIXED_HAZ, only_nums=True)
	# ==================================================================

	target = choice(normalized_MIXED)

	std_fraction_mixed, std_fraction_mixed_time = benchmark(
		std_fraction, target, normalized_MIXED)
	numpy_fraction_mixed, numpy_fraction_mixed_time = benchmark(
		numpy_fraction, target, normalized_MIXED)
	numagr_fraction_mixed, numagr_fraction_mixed_time = benchmark(
		numagr.frac_above, target, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE(
		f"Finding fraction above target in mixed values, target = {target}:"))

	print(f"{LIGHT_YELLOW('sum(x > target) / len(x)')} t = {std_fraction_mixed_time + normal_time}\n{std_fraction_mixed}")
	print(f"{LIGHT_YELLOW('numpy.mean(x > target)  ')} t = {numpy_fraction_mixed_time + normal_time}\n{numpy_fraction_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.frac_above       ')} t = {numagr_fraction_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_fraction_mixed}")

	print("Fraction above target benchmark completed", OK)

def mode_test():

	print(YELLOW("Mode benchmark..."))

	# defining counter mode ================================
	counter_mode = lambda X: Counter(X).most_common(1)[0][0]
	# ======================================================

	# injecting a mode
	mode = randint(1, LIMIT)
	INTS.extend([mode] * 1_000)

	counter_mode_ints, counter_mode_ints_time = benchmark(
		counter_mode, INTS)
	stat_mode_ints, stat_mode_ints_time = benchmark(
		statistics.mode, INTS)
	numagr_mode_ints, numagr_mode_ints_time = benchmark(
		numagr.mode, INTS)

	print(LIGHT_BLUE("Finding mode in integers:"))

	print(f"{LIGHT_YELLOW('Counter(x).most_common)')} t = {counter_mode_ints_time}\n{counter_mode_ints}")
	print(f"{LIGHT_YELLOW('statistics.mode        ')} t = {stat_mode_ints_time}\n{stat_mode_ints}")
	print(f"{LIGHT_GREEN( 'numagr.mode            ')} t = {numagr_mode_ints_time} {LIGHT_GREEN('<-')}\n{numagr_mode_ints}")

	mode = random()
	FLOATS.extend([mode] * 1_000)

	counter_mode_floats, counter_mode_floats_time = benchmark(
		counter_mode, FLOATS)
	stat_mode_floats, stat_mode_floats_time = benchmark(
		statistics.mode, FLOATS)
	numagr_mode_floats, numagr_mode_floats_time = benchmark(
		numagr.mode, FLOATS)

	print(LIGHT_BLUE("Finding mode in floats:"))

	print(f"{LIGHT_YELLOW('Counter(x).most_common)')} t = {counter_mode_floats_time}\n{counter_mode_floats}")
	print(f"{LIGHT_YELLOW('statistics.mode        ')} t = {stat_mode_floats_time}\n{stat_mode_floats}")
	print(f"{LIGHT_GREEN( 'numagr.mode            ')} t = {numagr_mode_floats_time} {LIGHT_GREEN('<-')}\n{numagr_mode_floats}")

	mode = random()
	MIXED_HAZ.extend([mode] * 1_000)

	# get normalized MIXED_HAZ==========================================
	normalized_MIXED, normal_time = get_xtime(
		normalize_mixed, MIXED_HAZ, only_nums=True)
	# ==================================================================

	counter_mode_mixed, counter_mode_mixed_time = benchmark(
		counter_mode, normalized_MIXED)
	stat_mode_mixed, stat_mode_mixed_time = benchmark(
		statistics.mode, normalized_MIXED)
	numagr_mode_mixed, numagr_mode_mixed_time = benchmark(
		numagr.mode, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE("Finding mode in mixed values:"))

	print(f"{LIGHT_YELLOW('Counter(x).most_common)')} t = {counter_mode_mixed_time + normal_time}\n{counter_mode_mixed}")
	print(f"{LIGHT_YELLOW('statistics.mode        ')} t = {stat_mode_mixed_time + normal_time}\n{stat_mode_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.mode            ')} t = {numagr_mode_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_mode_mixed}")

	print("Mode benchmark completed", OK)

def variance_test():

	print(YELLOW("Variance benchmark..."))

	# defining std variance ================================
	std_var = lambda X: std_variance(X, len(X) - 1)
	# ======================================================

	std_var_ints, std_var_ints_time = benchmark(
		std_var, INTS)
	stat_var_ints, stat_var_ints_time = benchmark(
		statistics.variance, INTS)
	numpy_var_ints, numpy_var_ints_time = benchmark(
		numpy.var, INTS, ddof = 1)
	numagr_var_ints, numagr_var_ints_time = benchmark(
		numagr.var, INTS)

	print(LIGHT_BLUE("Finding variance in integers:"))

	print(f"{LIGHT_YELLOW('sum((x - mean) ** 2) / (n - 1)')} t = {std_var_ints_time}\n{std_var_ints}")
	print(f"{LIGHT_YELLOW('statistics.variance           ')} t = {stat_var_ints_time}\n{stat_var_ints}")
	print(f"{LIGHT_YELLOW('numpy.var(X, ddof = 1)        ')} t = {numpy_var_ints_time}\n{numpy_var_ints}")
	print(f"{LIGHT_GREEN( 'numagr.var                    ')} t = {numagr_var_ints_time} {LIGHT_GREEN('<-')}\n{numagr_var_ints}")

	std_var_floats, std_var_floats_time = benchmark(
	std_variance, FLOATS, len(FLOATS) - 1)
	stat_var_floats, stat_var_floats_time = benchmark(
		statistics.variance, FLOATS)
	numpy_var_floats, numpy_var_floats_time = benchmark(
		numpy.var, FLOATS, ddof=1)
	numagr_var_floats, numagr_var_floats_time = benchmark(
		numagr.var, FLOATS)

	print(LIGHT_BLUE("Finding variance in floats:"))

	print(f"{LIGHT_YELLOW('sum((x - mean) ** 2) / (n - 1)')} t = {std_var_floats_time}\n{std_var_floats}")
	print(f"{LIGHT_YELLOW('statistics.variance           ')} t = {stat_var_floats_time}\n{stat_var_floats}")
	print(f"{LIGHT_YELLOW('numpy.var(X, ddof=1)          ')} t = {numpy_var_floats_time}\n{numpy_var_floats}")
	print(f"{LIGHT_GREEN( 'numagr.var                    ')} t = {numagr_var_floats_time} {LIGHT_GREEN('<-')}\n{numagr_var_floats}")

	# get normalized MIXED_HAZ ==========================================
	normalized_MIXED, normal_time = get_xtime(
		normalize_mixed, MIXED_HAZ, only_nums=True)
	# ==================================================================

	std_var_mixed, std_var_mixed_time = benchmark(
		std_variance, normalized_MIXED, len(normalized_MIXED) - 1)
	stat_var_mixed, stat_var_mixed_time = benchmark(
		statistics.variance, normalized_MIXED)
	numpy_var_mixed, numpy_var_mixed_time = benchmark(
		numpy.var, normalized_MIXED, ddof=1)
	numagr_var_mixed, numagr_var_mixed_time = benchmark(
		numagr.var, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE("Finding variance in mixed values:"))

	print(f"{LIGHT_YELLOW('sum((x - mean) ** 2) / (n - 1)')} t = {std_var_mixed_time + normal_time}\n{std_var_mixed}")
	print(f"{LIGHT_YELLOW('statistics.variance           ')} t = {stat_var_mixed_time + normal_time}\n{stat_var_mixed}")
	print(f"{LIGHT_YELLOW('numpy.var(X, ddof=1)          ')} t = {numpy_var_mixed_time + normal_time}\n{numpy_var_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.var                    ')} t = {numagr_var_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_var_mixed}")

	print("Variance benchmark completed", OK)

def pvariance_test():

	print(YELLOW("Population variance benchmark..."))

	# defining std population variance ======================
	std_pvar = lambda X: std_variance(X, len(X))
	# ======================================================

	std_pvar_ints, std_pvar_ints_time = benchmark(
		std_pvar, INTS)
	stat_pvar_ints, stat_pvar_ints_time = benchmark(
		statistics.pvariance, INTS)
	numpy_pvar_ints, numpy_pvar_ints_time = benchmark(
		numpy.var, INTS)
	numagr_pvar_ints, numagr_pvar_ints_time = benchmark(
		numagr.pvar, INTS)

	print(LIGHT_BLUE("Finding population variance in integers:"))

	print(f"{LIGHT_YELLOW('sum((x - mean) ** 2) / n')} t = {std_pvar_ints_time}\n{std_pvar_ints}")
	print(f"{LIGHT_YELLOW('statistics.pvariance    ')} t = {stat_pvar_ints_time}\n{stat_pvar_ints}")
	print(f"{LIGHT_YELLOW('numpy.var               ')} t = {numpy_pvar_ints_time}\n{numpy_pvar_ints}")
	print(f"{LIGHT_GREEN( 'numagr.pvar             ')} t = {numagr_pvar_ints_time} {LIGHT_GREEN('<-')}\n{numagr_pvar_ints}")

	std_pvar_floats, std_pvar_floats_time = benchmark(
		std_pvar, FLOATS)
	stat_pvar_floats, stat_pvar_floats_time = benchmark(
		statistics.pvariance, FLOATS)
	numpy_pvar_floats, numpy_pvar_floats_time = benchmark(
		numpy.var, FLOATS)
	numagr_pvar_floats, numagr_pvar_floats_time = benchmark(
		numagr.pvar, FLOATS)

	print(LIGHT_BLUE("Finding population variance in floats:"))

	print(f"{LIGHT_YELLOW('sum((x - mean) ** 2) / n')} t = {std_pvar_floats_time}\n{std_pvar_floats}")
	print(f"{LIGHT_YELLOW('statistics.pvariance    ')} t = {stat_pvar_floats_time}\n{stat_pvar_floats}")
	print(f"{LIGHT_YELLOW('numpy.var               ')} t = {numpy_pvar_floats_time}\n{numpy_pvar_floats}")
	print(f"{LIGHT_GREEN( 'numagr.pvar             ')} t = {numagr_pvar_floats_time} {LIGHT_GREEN('<-')}\n{numagr_pvar_floats}")

	# get normalized MIXED_HAZ ==========================================
	normalized_MIXED, normal_time = get_xtime(
		normalize_mixed, MIXED_HAZ, only_nums=True)
	# ==================================================================

	std_pvar_mixed, std_pvar_mixed_time = benchmark(
		std_pvar, normalized_MIXED)
	stat_pvar_mixed, stat_pvar_mixed_time = benchmark(
		statistics.pvariance, normalized_MIXED)
	numpy_pvar_mixed, numpy_pvar_mixed_time = benchmark(
		numpy.var, normalized_MIXED)
	numagr_pvar_mixed, numagr_pvar_mixed_time = benchmark(
		numagr.pvar, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE("Finding population variance in mixed values:"))

	print(f"{LIGHT_YELLOW('sum((x - mean) ** 2) / n')} t = {std_pvar_mixed_time + normal_time}\n{std_pvar_mixed}")
	print(f"{LIGHT_YELLOW('statistics.pvariance    ')} t = {stat_pvar_mixed_time + normal_time}\n{stat_pvar_mixed}")
	print(f"{LIGHT_YELLOW('numpy.var               ')} t = {numpy_pvar_mixed_time + normal_time}\n{numpy_pvar_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.pvar             ')} t = {numagr_pvar_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_pvar_mixed}")

	print("Population variance benchmark completed", OK)

def stdev_test():

	print(YELLOW("Standard deviation benchmark..."))

	# defining std standard deviation ========================
	std_stdev = lambda X: math.sqrt(std_variance(X, len(X) - 1))
	# ========================================================

	std_stdev_ints, std_stdev_ints_time = benchmark(
		std_stdev, INTS)
	stat_stdev_ints, stat_stdev_ints_time = benchmark(
		statistics.stdev, INTS)
	numpy_stdev_ints, numpy_stdev_ints_time = benchmark(
		numpy.std, INTS, ddof=1)
	numagr_stdev_ints, numagr_stdev_ints_time = benchmark(
		numagr.stdev, INTS)

	print(LIGHT_BLUE("Finding standard deviation in integers:"))

	print(f"{LIGHT_YELLOW('sqrt(sum((x - mean) ** 2) / (n - 1))')} t = {std_stdev_ints_time}\n{std_stdev_ints}")
	print(f"{LIGHT_YELLOW('statistics.stdev                    ')} t = {stat_stdev_ints_time}\n{stat_stdev_ints}")
	print(f"{LIGHT_YELLOW('numpy.std(X, ddof=1)                ')} t = {numpy_stdev_ints_time}\n{numpy_stdev_ints}")
	print(f"{LIGHT_GREEN( 'numagr.stdev                        ')} t = {numagr_stdev_ints_time} {LIGHT_GREEN('<-')}\n{numagr_stdev_ints}")

	std_stdev_floats, std_stdev_floats_time = benchmark(
		std_stdev, FLOATS)
	stat_stdev_floats, stat_stdev_floats_time = benchmark(
		statistics.stdev, FLOATS)
	numpy_stdev_floats, numpy_stdev_floats_time = benchmark(
		numpy.std, FLOATS, ddof=1)
	numagr_stdev_floats, numagr_stdev_floats_time = benchmark(
		numagr.stdev, FLOATS)

	print(LIGHT_BLUE("Finding standard deviation in floats:"))

	print(f"{LIGHT_YELLOW('sqrt(sum((x - mean) ** 2) / (n - 1))')} t = {std_stdev_floats_time}\n{std_stdev_floats}")
	print(f"{LIGHT_YELLOW('statistics.stdev                    ')} t = {stat_stdev_floats_time}\n{stat_stdev_floats}")
	print(f"{LIGHT_YELLOW('numpy.std(X, ddof=1)                ')} t = {numpy_stdev_floats_time}\n{numpy_stdev_floats}")
	print(f"{LIGHT_GREEN( 'numagr.stdev                        ')} t = {numagr_stdev_floats_time} {LIGHT_GREEN('<-')}\n{numagr_stdev_floats}")

	# get normalized MIXED_HAZ ==========================================
	normalized_MIXED, normal_time = get_xtime(
		normalize_mixed, MIXED_HAZ, only_nums=True)
	# ==================================================================

	std_stdev_mixed, std_stdev_mixed_time = benchmark(
		std_stdev, normalized_MIXED)
	stat_stdev_mixed, stat_stdev_mixed_time = benchmark(
		statistics.stdev, normalized_MIXED)
	numpy_stdev_mixed, numpy_stdev_mixed_time = benchmark(
		numpy.std, normalized_MIXED, ddof=1)
	numagr_stdev_mixed, numagr_stdev_mixed_time = benchmark(
		numagr.stdev, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE("Finding standard deviation in mixed values:"))

	print(f"{LIGHT_YELLOW('sqrt(sum((x - mean) ** 2) / (n - 1))')} t = {std_stdev_mixed_time + normal_time}\n{std_stdev_mixed}")
	print(f"{LIGHT_YELLOW('statistics.stdev                    ')} t = {stat_stdev_mixed_time + normal_time}\n{stat_stdev_mixed}")
	print(f"{LIGHT_YELLOW('numpy.std(X, ddof=1)                ')} t = {numpy_stdev_mixed_time + normal_time}\n{numpy_stdev_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.stdev                        ')} t = {numagr_stdev_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_stdev_mixed}")

	print("Standard deviation benchmark completed", OK)

def pstdev_test():

	print(YELLOW("Population standard deviation benchmark..."))

	# defining std population standard deviation =============
	std_pstdev = lambda X: math.sqrt(std_variance(X, len(X)))
	# ========================================================

	std_pstdev_ints, std_pstdev_ints_time = benchmark(
		std_pstdev, INTS)
	stat_pstdev_ints, stat_pstdev_ints_time = benchmark(
		statistics.pstdev, INTS)
	numpy_pstdev_ints, numpy_pstdev_ints_time = benchmark(
		numpy.std, INTS)
	numagr_pstdev_ints, numagr_pstdev_ints_time = benchmark(
		numagr.pstdev, INTS)

	print(LIGHT_BLUE("Finding population standard deviation in integers:"))

	print(f"{LIGHT_YELLOW('sqrt(sum((x - mean) ** 2) / n)')} t = {std_pstdev_ints_time}\n{std_pstdev_ints}")
	print(f"{LIGHT_YELLOW('statistics.pstdev             ')} t = {stat_pstdev_ints_time}\n{stat_pstdev_ints}")
	print(f"{LIGHT_YELLOW('numpy.std                     ')} t = {numpy_pstdev_ints_time}\n{numpy_pstdev_ints}")
	print(f"{LIGHT_GREEN( 'numagr.pstdev                 ')} t = {numagr_pstdev_ints_time} {LIGHT_GREEN('<-')}\n{numagr_pstdev_ints}")

	std_pstdev_floats, std_pstdev_floats_time = benchmark(
		std_pstdev, FLOATS)
	stat_pstdev_floats, stat_pstdev_floats_time = benchmark(
		statistics.pstdev, FLOATS)
	numpy_pstdev_floats, numpy_pstdev_floats_time = benchmark(
		numpy.std, FLOATS)
	numagr_pstdev_floats, numagr_pstdev_floats_time = benchmark(
		numagr.pstdev, FLOATS)

	print(LIGHT_BLUE("Finding population standard deviation in floats:"))

	print(f"{LIGHT_YELLOW('sqrt(sum((x - mean) ** 2) / n)')} t = {std_pstdev_floats_time}\n{std_pstdev_floats}")
	print(f"{LIGHT_YELLOW('statistics.pstdev             ')} t = {stat_pstdev_floats_time}\n{stat_pstdev_floats}")
	print(f"{LIGHT_YELLOW('numpy.std                     ')} t = {numpy_pstdev_floats_time}\n{numpy_pstdev_floats}")
	print(f"{LIGHT_GREEN( 'numagr.pstdev                 ')} t = {numagr_pstdev_floats_time} {LIGHT_GREEN('<-')}\n{numagr_pstdev_floats}")

	# get normalized MIXED_HAZ ==========================================
	normalized_MIXED, normal_time = get_xtime(
		normalize_mixed, MIXED_HAZ, only_nums=True)
	# ==================================================================

	std_pstdev_mixed, std_pstdev_mixed_time = benchmark(
		std_pstdev, normalized_MIXED)
	stat_pstdev_mixed, stat_pstdev_mixed_time = benchmark(
		statistics.pstdev, normalized_MIXED)
	numpy_pstdev_mixed, numpy_pstdev_mixed_time = benchmark(
		numpy.std, normalized_MIXED)
	numagr_pstdev_mixed, numagr_pstdev_mixed_time = benchmark(
		numagr.pstdev, MIXED_HAZ, only_num=True)

	print(LIGHT_BLUE("Finding population standard deviation in mixed values:"))

	print(f"{LIGHT_YELLOW('sqrt(sum((x - mean) ** 2) / n)')} t = {std_pstdev_mixed_time + normal_time}\n{std_pstdev_mixed}")
	print(f"{LIGHT_YELLOW('statistics.pstdev             ')} t = {stat_pstdev_mixed_time + normal_time}\n{stat_pstdev_mixed}")
	print(f"{LIGHT_YELLOW('numpy.std                     ')} t = {numpy_pstdev_mixed_time + normal_time}\n{numpy_pstdev_mixed}")
	print(f"{LIGHT_GREEN( 'numagr.pstdev                 ')} t = {numagr_pstdev_mixed_time} {LIGHT_GREEN('<-')}\n{numagr_pstdev_mixed}")

	print("Population standard deviation benchmark completed", OK)

def main_test():

	tests = (sum_test, min_test, max_test, amean_test,
			 gmean_test, hmean_test, nearest_test, count_test,
			 fraction_below_test, fraction_above_test, mode_test,
			 variance_test, pvariance_test, stdev_test, pstdev_test)

	for test in tests:
		print(CYAN("* * *"))
		test()
		print(CYAN("* * *"))

if __name__ == "__main__":

	load_lists()

	main_test()
	# ITERATIONS = 100
	# ret, t = benchmark(numagr.stdev, FLOATS)
	# print(f"numagr ret = {ret}, time = {t}")
 #
	# ITERATIONS = 1
	# ret, t = benchmark(statistics.stdev, FLOATS)
	# print(f"stat ret =   {ret}, time = {t}")

	sys.exit(0)
