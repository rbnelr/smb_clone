
union V4 {
	struct {
		T	x, y, z, w;
	};
	T		arr[4];
	
	T& operator[] (u32 i) {					return arr[i]; }
	constexpr T operator[] (u32 i) const {	return arr[i]; }
	
	INL V4 () {}
	INL constexpr V4 (T all):				x{all},	y{all},	z{all},	w{all} {}
	INL constexpr V4 (T x, T y, T z, T w):	x{x},	y{y},	z{z},	w{w} {}
	INL constexpr V4 (V2 v, T z, T w):		x{v.x},	y{v.y},	z{z},	w{w} {}
	INL constexpr V4 (V3 v, T w):			x{v.x},	y{v.y},	z{v.z},	w{w} {}
	
	INL constexpr V2 xy () const {			return V2(x,y) ;};
	INL constexpr V3 xyz () const {			return V3(x,y,z) ;};
	
#if !BOOLVEC
	V4& operator+= (V4 r) {					return *this = V4(x +r.x, y +r.y, z +r.z, w +r.w); }
	V4& operator-= (V4 r) {					return *this = V4(x -r.x, y -r.y, z -r.z, w -r.w); }
	V4& operator*= (V4 r) {					return *this = V4(x * r.x, y * r.y, z * r.z, w * r.w); }
	V4& operator/= (V4 r) {					return *this = V4(x / r.x, y / r.y, z / r.z, w / r.w); }
	
	#if FLTVEC
	operator u8v4() const;
	operator s64v4() const;
	operator s32v4() const;
	#endif
	#if INTVEC
	operator fv4() const {					return fv4((f32)x, (f32)y, (f32)z, (f32)w); }
	#endif
#endif
};

#if BOOLVEC
	VEC_STATIC constexpr bool all (V4 b) {				return b.x && b.y && b.z && b.w; }
	VEC_STATIC constexpr bool any (V4 b) {				return b.x || b.y || b.z || b.w; }
	
	VEC_STATIC constexpr V4 operator! (V4 b) {			return V4(!b.x,			!b.y,		!b.z,		!b.w); }
	VEC_STATIC constexpr V4 operator&& (V4 l, V4 r) {	return V4(l.x && r.x,	l.y && r.y,	l.z && r.z,	l.w && r.w); }
	VEC_STATIC constexpr V4 operator|| (V4 l, V4 r) {	return V4(l.x || r.x,	l.y || r.y,	l.z || r.z,	l.w || r.w); }
	VEC_STATIC constexpr V4 XOR (V4 l, V4 r) {			return V4(BOOL_XOR(l.x, r.x),	BOOL_XOR(l.y, r.y),	BOOL_XOR(l.z, r.z),	BOOL_XOR(l.w, r.w)); }
	
#else
	VEC_STATIC constexpr BV4 operator < (V4 l, V4 r) {	return BV4(l.x  < r.x,	l.y  < r.y,	l.z  < r.z,	l.w  < r.w); }
	VEC_STATIC constexpr BV4 operator<= (V4 l, V4 r) {	return BV4(l.x <= r.x,	l.y <= r.y,	l.z <= r.z,	l.w <= r.w); }
	VEC_STATIC constexpr BV4 operator > (V4 l, V4 r) {	return BV4(l.x  > r.x,	l.y  > r.y,	l.z  > r.z,	l.w  > r.w); }
	VEC_STATIC constexpr BV4 operator>= (V4 l, V4 r) {	return BV4(l.x >= r.x,	l.y >= r.y,	l.z >= r.z,	l.w >= r.w); }
	VEC_STATIC constexpr BV4 operator== (V4 l, V4 r) {	return BV4(l.x == r.x,	l.y == r.y,	l.z == r.z,	l.w == r.w); }
	VEC_STATIC constexpr BV4 operator!= (V4 l, V4 r) {	return BV4(l.x != r.x,	l.y != r.y,	l.z != r.z,	l.w != r.w); }
	VEC_STATIC constexpr V4 select (BV4 c, V4 l, V4 r) {
		return V4(	c.x ? l.x : r.x,	c.y ? l.y : r.y,	c.z ? l.z : r.z,	c.w ? l.w : r.w );
	}
	
	VEC_STATIC constexpr bool equal (V4 l, V4 r) {		return l.x == r.x && l.y == r.y && l.z == r.z && l.w == r.w; }
	
	VEC_STATIC constexpr V4 operator+ (V4 v) {			return v; }
	VEC_STATIC constexpr V4 operator- (V4 v) {			return V4(-v.x, -v.y, -v.z, -v.w); }

	VEC_STATIC constexpr V4 operator+ (V4 l, V4 r) {	return V4(l.x +r.x, l.y +r.y, l.z +r.z, l.w +r.w); }
	VEC_STATIC constexpr V4 operator- (V4 l, V4 r) {	return V4(l.x -r.x, l.y -r.y, l.z -r.z, l.w -r.w); }
	VEC_STATIC constexpr V4 operator* (V4 l, V4 r) {	return V4(l.x * r.x, l.y * r.y, l.z * r.z, l.w	* r.w); }
	VEC_STATIC constexpr V4 operator/ (V4 l, V4 r) {	return V4(l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w); }
	#if INTVEC
	VEC_STATIC constexpr V4 operator% (V4 l, V4 r) {	return V4(l.x % r.x, l.y % r.y, l.z % r.z, l.w % r.w); }
	#endif

	VEC_STATIC constexpr T dot(V4 l, V4 r) { return l.x*r.x + l.y*r.y + l.z*r.z + l.w*r.w; }

	VEC_STATIC V4 abs(V4 v) { return V4(abs(v.x), abs(v.y), abs(v.z), abs(v.w)); }
	VEC_STATIC T max_component(V4 v) { return max(max(v.x, v.y), max(v.z, v.w)); }

	VEC_STATIC constexpr V4 min(V4 l, V4 r) { return V4(min(l.x, r.x), min(l.y, r.y), min(l.z, r.z), min(l.w, r.w)); }
	VEC_STATIC constexpr V4 max(V4 l, V4 r) { return V4(max(l.x, r.x), max(l.y, r.y), max(l.z, r.z), max(l.w, r.w)); }
	
	VEC_STATIC constexpr V4 clamp (V4 val, V4 l, V4 h) {return min( max(val,l), h ); }
	
	#if FLTVEC
	VEC_STATIC constexpr V4 lerp (V4 a, V4 b, V4 t) {	return (a * (V4(1) -t)) +(b * t); }
	VEC_STATIC constexpr V4 map (V4 x, V4 in_a, V4 in_b) {	return (x -in_a)/(in_b -in_a); }
	VEC_STATIC constexpr V4 map (V4 x, V4 in_a, V4 in_b, V4 out_a, V4 out_b) {
													return lerp(out_a, out_b, map(x, in_a, in_b)); }
	
	VEC_STATIC T length (V4 v) {						return sqrt(v.x*v.x +v.y*v.y +v.z*v.z +v.w*v.w); }
	VEC_STATIC T length_sqr (V4 v) {					return v.x*v.x +v.y*v.y +v.z*v.z +v.w*v.w; }
	VEC_STATIC T distance (V4 a, V4 b) {				return length(b -a); }
	VEC_STATIC V4 normalize (V4 v) {					return v / V4(length(v)); }
	VEC_STATIC V4 normalize_or_zero (V4 v) { // TODO: epsilon?
		T len = length(v);
		if (len != 0) {
			 v /= len;
		}
		return v;
	}
	
	VEC_STATIC constexpr V4 to_deg (V4 v) {				return v * RAD_TO_DEG; }
	VEC_STATIC constexpr V4 to_rad (V4 v) {				return v * DEG_TO_RAD; }

	VEC_STATIC V4 mymod (V4 val, V4 range) {
		return V4(	mymod(val.x, range.x),
					mymod(val.y, range.y),
					mymod(val.z, range.z),
					mymod(val.w, range.w) );
	}
	
	VEC_STATIC V4 floor (V4 v) {						return V4(floor(v.x),	floor(v.y),	floor(v.z),	floor(v.w)); }
	VEC_STATIC V4 ceil (V4 v) {							return V4(ceil(v.x),	ceil(v.y),	ceil(v.z),	ceil(v.w)); }
	#endif
#endif
