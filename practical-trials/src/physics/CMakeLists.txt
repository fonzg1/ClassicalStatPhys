set(PROGS 
  toda_lattice
  )
foreach(name ${PROGS})
  add_executable(${name} ${name}.cpp)
  add_iotest(${name})
endforeach(name)
