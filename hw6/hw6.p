fof(a1, axiom,
     ! [X] : (likes(X, apples) => plays(X, chess))).
fof(a2, axiom,
     ! [X] : (likes(X, oranges) => plays(X, go))).
fof(a3, axiom,
     ! [X] : ((likes(X, oranges) & ~likes(X, apples)) | (likes(X, apples) & likes(X, oranges)))).
fof(a4, axiom,
     likes(john, apples)).
fof(a5, axiom,
     ! [X] : (likes(john, Y) => likes(mary, Y))).
fof(c1, conjecture, plays(mary, go)).