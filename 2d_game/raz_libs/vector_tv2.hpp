
union V2 {
	struct {
		T	x, y;
	};
	T		arr[2];
	
	T& operator[] (u32 i) {					return arr[i]; }
	constexpr T operator[] (u32 i) const {	return arr[i]; }
	
	INL V2 () {}
	INL constexpr V2 (T all):				x{all},	y{all} {}
	INL constexpr V2 (T x, T y):			x{x},	y{y} {}
	
#if !BOOLVEC
	V2& operator+= (V2 r) {					return *this = V2(x +r.x, y +r.y); }
	V2& operator-= (V2 r) {					return *this = V2(x -r.x, y -r.y); }
	V2& operator*= (V2 r) {					return *this = V2(x * r.x, y * r.y); }
	V2& operator/= (V2 r) {					return *this = V2(x / r.x, y / r.y); }
	
	#if FLTVEC
	operator u8v2() const;
	operator s64v2() const;
	operator s32v2() const;
	#endif
	#if INTVEC
	operator fv2() const {					return fv2((f32)x, (f32)y); }
	#endif
#endif
};

#if BOOLVEC
	VEC_STATIC constexpr bool all (V2 b) {				return b.x && b.y; }
	VEC_STATIC constexpr bool any (V2 b) {				return b.x || b.y; }
	
	VEC_STATIC constexpr V2 operator! (V2 b) {			return V2(!b.x,			!b.y); }
	VEC_STATIC constexpr V2 operator&& (V2 l, V2 r) {	return V2(l.x && r.x,	l.y && r.y); }
	VEC_STATIC constexpr V2 operator|| (V2 l, V2 r) {	return V2(l.x || r.x,	l.y || r.y); }
	VEC_STATIC constexpr V2 XOR (V2 l, V2 r) {			return V2(BOOL_XOR(l.x, r.x),	BOOL_XOR(l.y, r.y)); }
	
#else
	VEC_STATIC constexpr BV2 operator < (V2 l, V2 r) {	return BV2(l.x  < r.x,	l.y  < r.y); }
	VEC_STATIC constexpr BV2 operator<= (V2 l, V2 r) {	return BV2(l.x <= r.x,	l.y <= r.y); }
	VEC_STATIC constexpr BV2 operator > (V2 l, V2 r) {	return BV2(l.x  > r.x,	l.y  > r.y); }
	VEC_STATIC constexpr BV2 operator>= (V2 l, V2 r) {	return BV2(l.x >= r.x,	l.y >= r.y); }
	VEC_STATIC constexpr BV2 operator== (V2 l, V2 r) {	return BV2(l.x == r.x,	l.y == r.y); }
	VEC_STATIC constexpr BV2 operator!= (V2 l, V2 r) {	return BV2(l.x != r.x,	l.y != r.y); }
	VEC_STATIC constexpr V2 select (BV2 c, V2 l, V2 r) {
		return V2(	c.x ? l.x : r.x,	c.y ? l.y : r.y );
	}
	
	VEC_STATIC constexpr bool equal (V2 l, V2 r) {		return l.x == r.x && l.y == r.y; }
	
	VEC_STATIC constexpr V2 operator+ (V2 v) {			return v; }
	VEC_STATIC constexpr V2 operator- (V2 v) {			return V2(-v.x, -v.y); }
	
	VEC_STATIC constexpr V2 operator+ (V2 l, V2 r) {	return V2(l.x +r.x, l.y +r.y); }
	VEC_STATIC constexpr V2 operator- (V2 l, V2 r) {	return V2(l.x -r.x, l.y -r.y); }
	VEC_STATIC constexpr V2 operator* (V2 l, V2 r) {	return V2(l.x * r.x, l.y * r.y); }
	VEC_STATIC constexpr V2 operator/ (V2 l, V2 r) {	return V2(l.x / r.x, l.y / r.y); }
	#if INTVEC
	VEC_STATIC constexpr V2 operator% (V2 l, V2 r) {	return V2(l.x % r.x, l.y % r.y); }
	#endif
	
	VEC_STATIC constexpr T dot(V2 l, V2 r) { return l.x*r.x + l.y*r.y; }

	VEC_STATIC V2 abs(V2 v) { return V2(abs(v.x), abs(v.y)); }
	VEC_STATIC T max_component(V2 v) { return max(v.x, v.y); }

	VEC_STATIC constexpr V2 min(V2 l, V2 r) { return V2(min(l.x, r.x), min(l.y, r.y)); }
	VEC_STATIC constexpr V2 max(V2 l, V2 r) { return V2(max(l.x, r.x), max(l.y, r.y)); }

	VEC_STATIC constexpr V2 clamp (V2 val, V2 l, V2 h) {return min( max(val,l), h ); }
	
	#if FLTVEC
	VEC_STATIC constexpr V2 lerp (V2 a, V2 b, V2 t) {	return (a * (V2(1) -t)) +(b * t); }
	VEC_STATIC constexpr V2 map (V2 x, V2 in_a, V2 in_b) {	return (x -in_a)/(in_b -in_a); }
	VEC_STATIC constexpr V2 map (V2 x, V2 in_a, V2 in_b, V2 out_a, V2 out_b) {
													return lerp(out_a, out_b, map(x, in_a, in_b)); }
	
	VEC_STATIC T length (V2 v) {						return sqrt(v.x*v.x +v.y*v.y); }
	VEC_STATIC T length_sqr (V2 v) {					return v.x*v.x +v.y*v.y; }
	VEC_STATIC T distance (V2 a, V2 b) {				return length(b -a); }
	VEC_STATIC V2 normalize (V2 v) {					return v / V2(length(v)); }
	VEC_STATIC V2 normalize_or_zero (V2 v) { // TODO: epsilon?
		T len = length(v);
		if (len != 0) {
			 v /= len;
		}
		return v;
	}
	
	VEC_STATIC constexpr V2 to_deg (V2 v) {				return v * RAD_TO_DEG; }
	VEC_STATIC constexpr V2 to_rad (V2 v) {				return v * DEG_TO_RAD; }
	
	VEC_STATIC V2 mymod (V2 val, V2 range) {
		return V2(	mymod(val.x, range.x),	mymod(val.y, range.y) );
	}

	VEC_STATIC V2 floor (V2 v) {						return V2(floor(v.x),	floor(v.y)); }
	VEC_STATIC V2 ceil (V2 v) {							return V2(ceil(v.x),	ceil(v.y)); }
	#endif
#endif
