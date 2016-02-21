 <h1 class="title">Assignment 2   </h1>
<div class="paragraphs"><p>In this assignment, you will solve problems involving multiple agents.</p>
<ul>
<li>T1: (Reaching a stationary formation) Given starting positions of N vehicles, and a formation description (see below), make the vehicles reach the formation as fast as possible. (ignore vehicle-vehicle collisions)
<ul>
<li>A typical formation could be the 4-4-2 (<a href="https://en.wikipedia.org/wiki/Formation_(association_football)">link</a>) soccer formation: (0,0), (1,0), (2,0), (3,0), (0,1), (1,1), (2,1), (3,1), (1,2), (2,2), where each of the 10 vehicle positions (Nx,Ny) are given in multiples of some separation distance d. (for a real soccer field, d=15 would give positions (0,0), (15,0), (30,0), (45,0), (0,15), (15,15) ... in meters)</li>
<li>T1a: solve the problem when every agent has knowledge of the location of all other agents.</li>
<li>T1b: solve the problem when every agent only has knowledge of neighbors within a radius R</li>
</ul>
</li>
<li>T2: (Reaching a moving formation) The same as T1, with the exception that the final formation should be moving at a given speed in a given direction. (ignore vehicle-vehicle collisions)</li>
<li>T3: (VRP in polygonal map): Given a polygonal map, as in Assignment 1, starting positions of N vehicles and M customers, find paths for all N vehicles such that all M customers are visited in (approximately) minimum time. (Ignore possible collisions in this task)</li>
<li>T4: (Obstacle avoidance in empty space) Given an empty space, N vehicle starting positions, and a given destination for each vehicle. Solve the problem of reaching that destination without colliding with the other vehicles. Solve the problem in a decentralized/reactive manner (i.e. each vehicle has no knowledge of the starting positions or destinations of the others). Assume that the vehicles occupy circular discs with radii R.</li>
<li>T5: (mini Darpa Urban Challenge) Solve the same problem as above in a polygonal environment.</li>
<li>T6: Report in the form of a scientific paper. Also include discussion of performance relative to other groups (info from wiki-page and presentations).</li>
<li>T7: Be ready to show your solutions at the final meeting.</li>
</ul>
<h2>Planning and Progress reporting</h2>
<p>Very similar to Assignment 1. Use the wiki-pages.</p>
<h2>Example problems</h2>
<p>A polygonal <strong>obstacle environment</strong>, including starting (*), ending (x) and customer (o) positions, can be found in this file <a href="https://www.kth.se/social/files/54f71e5df2765446e082d1cc/polygObst.mat" title="polygObst.mat (x-matlab-workspace)">polygObst.mat</a>, which can be plotted using <a href="https://www.kth.se/social/files/54f71e9cf2765446ac6c5bd9/plotPolygObst.m" title="plotPolygObst.m (octet-stream)">plotPolygObst.m</a>. The environment can be seen here <a href="https://www.kth.se/social/files/54f71eeaf276544d8ee0624a/polygonalMap.pdf" title="map.pdf (pdf)">map.pdf</a> (the lines connecting start and end points indicate who goes where).</p>
<p>An <strong>empty environment</strong>, with starting and end positions, can be found in this file <a href="https://www.kth.se/social/files/54f87880f2765468fbb9ef21/polygObst2.mat" title="polygObst2.mat (x-matlab-workspace)">polygObst2.mat</a>, which can be plotted using <a href="https://www.kth.se/social/files/54f878aaf2765478778cf734/plotPolygObst2.m" title="plotPolygObst2.m (octet-stream)">plotPolygObst2.m</a>. The environment can be seen here <a href="https://www.kth.se/social/files/54f878d7f276546921dce381/polygonalMap2.pdf" title="polygonalMap2.pdf (pdf)">polygonalMap2.pdf</a> (the lines connecting start and end points indicate who goes where).</p>
<ul>
<li>P1a: Solve T1 above, with the given 4-4-2 formation, d=50, and the 10 starting positions given by the 5 starting (*) and 5 ending (x) positions of polygObst above. The vehicle model is the dynamic point, with (Amax=1, Vmax=100).</li>
<li>P1b: Same as above, with the dynamic car (Amax=1, phimax=1)</li>
<li>P2a: Solve T2 above, with the same data as P1. The formation should move &quot;north&quot; with velocity 5.</li>
<li>P2b: Same as above, with the dynamic car (Amax=1, phimax=1)</li>
<li>P3a: Use the obstacle environment above to solve T3 (ignoring the destinations) using the dynamic point mass (Amax=1, Vmax=100)</li>
<li>P3b: Use the obstacle environment above to solve T3 using the dynamic car (Amax=1, phimax=1)</li>
<li>P4: Use the empty environment above to solve T4 using the dynamic point mass (Amax=1, Vmax=100, vehicle shape is disc with radius 1m)</li>
<li>P5a: Use the obstacle environment above to solve T5 (ignoring the customers) using the dynamic point mass (Amax=1, Vmax=100, vehicle shape is disc with radius 5m)</li>
<li>P5b: Use the obstacle environment above to solve T5 using the dynamic car (Amax=1, phimax=1,vehicle shape is disc with radius 5m)</li>
</ul>
<p></p>
<h2>Links </h2>
<ul>
<li>Cooperative Pathfinding, David Silver (http://www0.cs.ucl.ac.uk/staff/D.Silver/web/Applications_files/coop-path-AIWisdom.pdf)</li>
<li>Obstacle Avoidance in Formation (<a href="http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.93.8934&amp;rep=rep1&amp;type=pdf">link</a>)</li>
<li>A Control Scheme for Improving Multi-Vehicle Foration Maneuvers (<a href="http://www.et.byu.edu/~beard/papers/preprints/YoungBeardKelsey01a.pdf">link</a>)</li>
<li>Cooperative Control of Mobile Sensor Networks: Adaptive Gradient Climbing in a Distributed Environment (<a href="http://www.princeton.edu/~naomi/OgrFioLeoTAC04.pdf">link</a>)</li>
<li>Behavior-based formation control for multirobot teams (<a href="http://www.cim.mcgill.ca/~junaed/765/uploaded_talks/A_Kashi_Bahavior_based_formation.pdf">link</a>)</li>
</ul>
<ul>
<li>
<p>P. Toth and D. Vigo, “The Granular Tabu Search and Its Application to the Vehicle-Routing Problem,” <em>INFORMS Journal on Computing</em>, vol. 15, no. 4, pp. 333–346, Dec. 2003. (sorry, no link)</p>
</li>
<li>Solving the Vehicle Routing Problem with Genetic Algorithms, Áslaug Sóley Bjarnadóttir  (<a href="http://etd.dtu.dk/thesis/154736/imm3183.pdf">link</a>)</li>
<li>Proportional Navigation (for min time intercept, reverse for collision avoidance) (<a href="http://web.mit.edu/16.070/www/project/PG_missile_navigation.pdf">link</a>)</li>
<li>Cooperative Path finding (<a href="http://www.aaai.org/Papers/AIIDE/2005/AIIDE05-020.pdf">link</a>) (suggested by Lucas)</li>
<li>Reciprocal Velocity Obstacles for Realtime Multi-Agent Navigation (<a href="http://gamma.cs.unc.edu/RVO/">link</a>) (suggested by Dario)</li>
<li></li>
</ul></div>
