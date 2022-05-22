<!-- App.svelte -->
<script>
	import { onMount } from 'svelte';
	import { Router, Link, Route } from "svelte-routing";
	import Home from "./components/newtune/Home.svelte";	  
	import SelectScan from "./components/newtune/SelectScan.svelte";
	import Neerhome from "./components/neer/Neerhome.svelte";
	import Dashboard from "./components/Landing/Dashboard.svelte";
	//import queryString from "query-string";

	
	export let url = "";
	let parsed = {};
	let modejson ={message:"CHECKING"};
/*
  if (typeof window !== 'undefined') {
    parsed = queryString.parse(window.location.search);
  }
*/
	onMount(async () => {
		const res = await fetch(`/espnewtune/wifimode`);
		console.log(res);
		if (res.status === 200) { 
			modejson = await res.json();			
		}
	});

  </script>
  <p>{JSON.stringify(modejson)}</p>
  <Router url="{url}">
	<nav>
	  <Link to="/">Home</Link>
	  <Link to="/neer">Neer</Link>
	</nav>
	<div>
		{#if ["WIFI_AP","WIFI_STA_AP"].includes(modejson.message)}
			<Route path="/"><Home /></Route>
		{:else if (modejson.message !== "CHECKING") }
			<Route path="/"><Dashboard /></Route>
		{:else}
			<!-- TODO: Put a Loading page-->
		{/if}
		<Route path="/neer"><Neerhome /></Route>
	  
	</div>
  </Router>

  <!--
	Build size on May 01st
	css - 11.3 kb  (11.3)
	js - 173.6kb   (39.4)
	html - 379b    (379b)
  -->
  <style lang="postcss" global>
	@tailwind base;
	@tailwind components;
	@tailwind utilities;
  </style>


