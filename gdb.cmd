handle SIGSTOP nostop
echo ==================== STARTING RUN ==========================================\n
run
echo ======================= BACKTRACE ==========================================\n
thread apply all bt
quit

