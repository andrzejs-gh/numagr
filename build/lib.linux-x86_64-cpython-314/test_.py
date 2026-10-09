import numagr
import math
from random import randint, random
import time
import numpy
import statistics
import sys

def generator_from_floats(floats):
	for i in floats:
		yield i

def numpy_test(floats):
	t1 = time.perf_counter()
	s = numpy.asarray(floats, dtype=numpy.float64).mean()
	# s = numpy.sum()
	t2 = time.perf_counter()
	print("time [floats]:", t2 - t1)

def numagr_gmean_test(floats):
	t1 = time.perf_counter()
	s = numagr.gmean(floats)
	t2 = time.perf_counter()
	print("time [numagr gmean]:", t2 - t1)
	return s

def stat_gmean_test(floats):
	t1 = time.perf_counter()
	s = statistics.geometric_mean(floats)
	t2 = time.perf_counter()
	print("time [stat.  gmean]:", t2 - t1)
	return s

def numagr_hmean_test(floats):
	t1 = time.perf_counter()
	s = numagr.hmean(floats)
	t2 = time.perf_counter()
	print("time [numagr hmean]:", t2 - t1)
	return s

def stat_hmean_test(floats):
	t1 = time.perf_counter()
	s = statistics.harmonic_mean(floats)
	t2 = time.perf_counter()
	print("time [stat   hmean]:", t2 - t1)
	return s

def sum_test(floats):
	t1 = time.perf_counter()
	s = sum(floats)
	t2 = time.perf_counter()
	print("time [plain  sum]:", t2 - t1)
	return s

def stat_sum_test(floats):
	t1 = time.perf_counter()
	s = statistics.fsum(floats)
	t2 = time.perf_counter()
	print("time [stat. fsum]:", t2 - t1)
	return s

def numagr_sum_test(floats):
	t1 = time.perf_counter()
	s = numagr.sum(floats)
	t2 = time.perf_counter()
	print("time [numagr sum]:", t2 - t1)
	return s

def math_sum_test(floats):
	t1 = time.perf_counter()
	s = math.fsum(floats)
	t2 = time.perf_counter()
	print("time [math   fsum]:", t2 - t1)
	return s

def numagr_amean_test(floats):
	t1 = time.perf_counter()
	s = numagr.amean(floats)
	t2 = time.perf_counter()
	print("time [numagr amean]:", t2 - t1)
	return s

def stat_fmean_test(floats):
	t1 = time.perf_counter()
	s = statistics.fmean(floats)
	t2 = time.perf_counter()
	print("time [stat   fmean]:", t2 - t1)
	return s

def general_test():
	# random_floats = [str(random.random()) for _ in range(1000000)]
	# generator_test(random_floats)
	# numpy_test(random_floats)
	floats = [random() for _ in range(1_000_000)]

	plain_sum = sum_test(floats)
	stat_sum = stat_sum_test(floats)
	math_sum = math_sum_test(floats)
	numagr_sum = numagr_sum_test(floats)

	print("sum         =", plain_sum)
	print("stat_sum    =", stat_sum)
	print("math_fsum   =", math_sum)
	print("numagr_sum  =", numagr_sum)
# # # # # # # # # # # # # # # # # # # # # # #
	print("* * *")
	stat_fmean = stat_fmean_test(floats)
	numagr_amean = numagr_amean_test(floats)
	actual_amean = math_sum / len(floats)

	stat_gmean = stat_gmean_test(floats)
	numagr_gmean = numagr_gmean_test(floats)
	actual_gmean = math.exp(math.fsum((math.log(x) for x in floats)) / len(floats))

	stat_hmean = stat_hmean_test(floats)
	numagr_hmean = numagr_hmean_test(floats)
	min_val = min(floats)
	recip_sum = math.fsum(((min_val / x) for x in floats))
	actual_hmean = (len(floats)*min_val) / recip_sum

	print("-")

	print("stat_fmean    =", stat_fmean)
	print("numagr_amean  =", numagr_amean)
	print("actual_amean  =", actual_amean)
	print("-")

	print("stat_gmean    =", stat_gmean)
	print("numagr_gmean  =", numagr_gmean)
	print("actual_gmean  =", actual_gmean)
	print("-")

	print("stat_hmean    =", stat_hmean)
	print("numagr_hmean  =", numagr_hmean)
	print("actual_hmean  =", actual_hmean)

def generator_test():
	floats = [randint(1,10) for _ in range(1_000_000)]
	print("amean: ", numagr.amean((x for x in floats)))
	print("gmean: ", numagr.gmean((x for x in floats)))
	print("hmean: ", numagr.hmean((x for x in floats)))

def frac_test():
	floats = [randint(1,10) for _ in range(1_000_000)]
	below = numagr.frac_below(5.0, (x for x in floats))
	above = numagr.frac_above(5.0, (x for x in floats))
	print("below: ", below)
	print("above: ", above)

def mode_test():
	floats = [randint(1,10) for _ in range(1_000_000)]
	gen = (x for x in floats)

	t1 = time.perf_counter()
	numagr_mode = numagr.mode(33)
	t2 = time.perf_counter()
	numagr_mode_time = t2 - t1

	gen = (x for x in floats)
	t1 = time.perf_counter()
	stat_mode = statistics.mode(floats)
	t2 = time.perf_counter()
	stat_mode_time = t2 - t1

	print("numgr mode = ", numagr_mode, "t = ", numagr_mode_time)
	print("stat mode  = ", float(stat_mode), "t = ", stat_mode_time)

def hmeans():
	floats = [str(random()) for _ in range(1_000_000)]
	floats.extend((str(-0.2), str(-3.77)))

	t1 = time.perf_counter()
	hmean_arr = numagr.hmean_arr(floats)
	t2 = time.perf_counter()
	hmean_arr_time = t2 - t1

	t1 = time.perf_counter()
	hmean = numagr.hmean(floats)
	t2 = time.perf_counter()
	hmean_time = t2 - t1

	t1 = time.perf_counter()
	stat_hmean = statistics.harmonic_mean([float(x) for x in floats])
	t2 = time.perf_counter()
	stat_hmean_time = t2 - t1

	print("hmean      = ", hmean, "t = ", hmean_time)
	print("hmean_arr  = ", hmean_arr, "t = ", hmean_arr_time)
	print("stat       = ", stat_hmean, "t = ", stat_hmean_time)

def variance_test():
	floats = [random() for _ in range(1_000_000)]

	t1 = time.perf_counter()
	stat_var = statistics.variance(floats)
	t2 = time.perf_counter()
	stat_var_time = t2 - t1

	t1 = time.perf_counter()
	numagr_var = numagr.var(floats)
	t2 = time.perf_counter()
	numagr_var_time = t2 - t1

	t1 = time.perf_counter()
	numpy_var = numpy.var(floats, ddof=1)
	t2 = time.perf_counter()
	numpy_var_time = t2 - t1

	t1 = time.perf_counter()
	fm = math.fsum(floats) / len(floats)
	s = math.fsum((fm - x)*(fm - x) for x in floats)
	var = s / (len(floats)-1)
	t2 = time.perf_counter()
	var_time = t2 - t1

	print("stat_var   = ", stat_var, "t = ", stat_var_time)
	print("numagr_var = ", numagr_var, "t = ", numagr_var_time)
	print("numpy_var  = ", numpy_var, "t = ", numpy_var_time)
	print("var_       = ", var, "t = ", var_time)

def stdev_test():
	floats = [random() for _ in range(1_000_000)]

	t1 = time.perf_counter()
	stat_stdev = statistics.stdev(floats)
	t2 = time.perf_counter()
	stat_stdev_time = t2 - t1

	t1 = time.perf_counter()
	numagr_stdev = numagr.stdev(floats)
	t2 = time.perf_counter()
	numagr_stdev_time = t2 - t1

	t1 = time.perf_counter()
	numpy_stdev = numpy.std(floats, ddof=1)
	t2 = time.perf_counter()
	numpy_stdev_time = t2 - t1

	# t1 = time.perf_counter()
	# fm = math.fsum(floats) / len(floats)
	# s = math.fsum((fm - x)*(fm - x) for x in floats)
	# stdev = s / (len(floats)-1)
	# t2 = time.perf_counter()
	# stdev_time = t2 - t1

	print("stat_stdev   = ", stat_stdev, "t = ", stat_stdev_time)
	print("numagr_stdev = ", numagr_stdev, "t = ", numagr_stdev_time)
	print("numpy_stdev  = ", numpy_stdev, "t = ", numpy_stdev_time)
	# print("var_       = ", var, "t = ", var_time)

if __name__ == "__main__":
	# generator_test()
	# general_test()
	# frac_test()
	# mode_test()
	stdev_test()

