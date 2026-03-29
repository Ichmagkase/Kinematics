using Godot;

public partial class ToolTipLabel : Label
{
	private Tween _tween;

	public override void _Ready()
	{
		PivotOffset = Size / 2; // Expand from middle instead of top left
		StartPulse();
	}

	private void StartPulse()
	{
		_tween = CreateTween().SetLoops();
		_tween.TweenProperty(this, "scale", new Vector2(1.1f, 1.1f), 3);
		_tween.TweenProperty(this, "scale", new Vector2(1.0f, 1.0f), 3);
	}
}
