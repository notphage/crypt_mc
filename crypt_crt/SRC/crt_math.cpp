template <typename T>
T abs_template(T t)
{
	return t > (T)0 ? t : -t;
}

float fabs(float f)
{
	return abs_template<float>(f);
}

extern "C" float copysignf(float x, float y)
{
	return y == 0.f ? fabs(x) : fabs(x) * y / fabs(y);
}

extern "C" double log2(double a)
{
	union { double d; long long x; } u = { a };
	return (u.x - 4607182418800017409) * 1.539095918623324e-16; /* 1 / 6497320848556798.0; */
}

typedef union
{
  float value;
  unsigned int word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */
# define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0)

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d, i)                                     \
do {                                                            \
  ieee_float_shape_type sf_u;                                   \
  sf_u.word = (i);                                              \
  (d) = sf_u.value;                                             \
} while (0)

extern "C" float roundf(float x)
{
	int signbit;
	unsigned int w;
	/* Most significant word, least significant word. */
	int exponent_less_127;

	GET_FLOAT_WORD(w, x);

	/* Extract sign bit. */
	signbit = w & 0x80000000;

	/* Extract exponent field. */
	exponent_less_127 = (int)((w & 0x7f800000) >> 23) - 127;

	if (exponent_less_127 < 23)
	{
		if (exponent_less_127 < 0)
		{
			w &= 0x80000000;
			if (exponent_less_127 == -1)
				/* Result is +1.0 or -1.0. */
				w |= ((unsigned int)127 << 23);
		}
		else
		{
			unsigned int exponent_mask = 0x007fffff >> exponent_less_127;
			if ((w & exponent_mask) == 0)
				/* x has an integral value. */
				return x;

			w += 0x00400000 >> exponent_less_127;
			w &= ~exponent_mask;
		}
	}
	else
	{
		if (exponent_less_127 == 128)
			/* x is NaN or infinite. */
			return x + x;
		else
			return x;
	}
	SET_FLOAT_WORD(x, w);
	return x;
}


