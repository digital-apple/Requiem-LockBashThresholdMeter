class BashMenu extends MovieClip
{
	var MeterContainer: MovieClip;
	
	var Bar: Color;
	
	var Counter: TextField;

	var MeterWidthMin: Number;
	var MeterWidthMax: Number;
	var MeterPercent: Number;
	
	function BashMenu()
	{
		super();
		
		Bar = new Color(MeterContainer.Bar);
		
		Counter = MeterContainer.Counter;
		
		MeterWidthMin = MeterContainer.Mask._x - MeterContainer.Mask._width;
		MeterWidthMax = MeterContainer.Mask._x;
		
		MeterPercent = (MeterWidthMax - MeterWidthMin) / 100;
	}
	
	function onLoad(): Void
	{
		this.gotoAndStop("Hide");
	}
	
	public function ApplyLayout(a_positionX: Number, a_positionY: Number, a_rotation: Number, a_scaleX: Number, a_scaleY: Number, a_alpha: Number): Void
	{
		this._x = a_positionX;
		this._y = a_positionY;
		this._rotation = a_rotation;
		this._xscale = a_scaleX;
		this._yscale = a_scaleY;
		this._alpha = a_alpha;
	}
	
	public function FlashMeter(): Void
	{
		if (this._currentframe == 1) 
		{
			this.gotoAndPlay("Flash");
		}
	}
	
	public function GetClampedValue(a_value: Number): Number
	{
		return a_value > 100 ? 100 : (a_value <= 0 ? -1 : a_value);
	}
	
	public function Hide(): Void
	{
		if (this._currentframe == 1 || this._currentframe > 2 )
		{
			this.gotoAndStop("Hide");
		}
	}
	
	public function SetMeterPercentage(a_percent: Number): Void
	{
		a_percent = GetClampedValue(a_percent);
		MeterContainer.Mask._x = MeterWidthMin + (MeterPercent * a_percent);	
		
	}
	
	public function Show(): Void
	{
		if (this._currentframe == 2)
		{
			this.gotoAndStop("Show");
		}
	}
	
	public function UpdateColor(a_color: Number): Void
	{
		Bar.setRGB(a_color);
	}
	
	public function UpdateCounter(a_enabled: Boolean, a_strength: Number, a_threshold: Number): Void
	{
		if (a_enabled) 
		{
			Counter.text = a_strength + " / " + a_threshold; 
		}
		else
		{
			Counter._alpha = 0;
		}
	}
}