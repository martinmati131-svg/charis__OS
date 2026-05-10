# Industrial VLA Logic Gate
def on_visual_input(camera_data, sop_context):
    """
    V = Visual (Batch Turbidity)
    L = Language (SOP Constraints)
    A = Action (Motor RPM)
    """
    if camera_data.turbidity > 0.8 and sop_context.stage == "Maceration":
        # Action: Increase stirring to prevent sedimentation
        return Action(cmd="STIR_FAST", rpm=150)
    
    if camera_data.color == "Vibrant_Yellow" and sop_context.target == "Tithonia":
        # Action: Nutrient peak reached, stop extraction
        return Action(cmd="SIGNAL_BATCH_COMPLETE", notification="RSS_PUSH")
