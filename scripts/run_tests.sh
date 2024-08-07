cleanup() { 
   ./remove_executables.sh # remove executables after program stopped by Ctrl+C
    kill $FOO_PID
   echo "\n"
}
trap cleanup SIGINT 

cd ..
./copy_executables.sh
./application --test --static_path static --postgres_credentials "postgres:1234" & 
FOO_PID=$!
sleep 1
./test_application $@
kill $FOO_PID
./remove_executables.sh