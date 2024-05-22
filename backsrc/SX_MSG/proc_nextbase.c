void nextbase(void)
{
	struct SXFuncStruct
	{
		UWORD	id;
		char	*string;
		UWORD	extra;
		UWORD	low;
		UWORD	high;
		char	*mainarg;
		char	*execarg;
	} SXFunc;

	SXFunc.id = 74;	// Next Msg Base
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = 0;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = 0;
	SXFunc.execarg = 0;

	XIMFunction(SX_FUNCTION, (long)&SXFunc, 0);
}

