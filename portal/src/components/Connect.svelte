<script>
  import { onMount, createEventDispatcher } from 'svelte';
  const dispatch = createEventDispatcher();

  
  export let selecteddt ={};
  let ssid;
  let direct_connect;  
  let orgpassword;  
  let loading = false;
  let password = "";
  let inlist = false;
  let storedformdata = null;

  async function connect(){

    loading = true;
    if(storedformdata == null){
      let formData = new FormData();
      formData.append('ssid', ssid);
      formData.append('password', password);

      const timezone = Intl.DateTimeFormat().resolvedOptions().timeZone;
      formData.append('timezone', timezone);

      if(inlist) {
        if(direct_connect){
          formData.append('optyp', "CON");
        } else {
          if(password == orgpassword){
            formData.append('optyp', "CON");
        } else {
          formData.append('optyp', "UPD");
        }
        }
      } else {
        formData.append('optyp', "ADD");
      }
      storedformdata = formData;    
    }

    const res = await fetch(`/espnewtune/connect`, { method: 'POST', body: storedformdata });
    console.log(res);
    const parsedJson = await res.json();
    
		if (res.status === 200) {      
      storedformdata = null;
      console.log(parsedJson);
      if(parsedJson.code === 'FAI'){        
        dispatch('error',{data:parsedJson});
      } else {        
        dispatch('success',{data:parsedJson});
      }
      
		} else if(res.status === 202) {
			setTimeout(connect(), 2000);
		} else {           
      storedformdata = null; 
      dispatch('error',{data:parsedJson});
    }
    loading = false;
		return res;
  }


  onMount(async () => {

    ssid = selecteddt.ssid;
    direct_connect = selecteddt.direct_connect;
    password=selecteddt.password;
    orgpassword=selecteddt.password;
    inlist = selecteddt.inlist;


    if(direct_connect){
      loading = true;
      await connect();
      loading = false;
    }
  })
</script>

<form class="flex flex-col mb-2 min-w-full p-2 w-auto"  on:submit|preventDefault={connect}>      
  <div class="flex flex-row justify-center h-16 text-blue-800">
    <p class="text-2xl">Connect to WiFi</p>
  </div>           
  <div class="flex flex-row flex-grow h-16 mb-10">    
      <input 
      class = "text-black text-xl"
      type="text" placeholder="SSID" id="ssid" value={ssid} disabled required>
  </div>
  <div class="flex flex-row flex-grow  h-8 mb-10">
    <input 
    class="mt-0 block w-full border-0 border-b-2 border-gray-200 focus:ring-0 focus:ring-offset-0 focus:border-blue hover:border-blue hover:border-b"
    type="password" placeholder="WiFi Password" id="password" bind:value={password} disabled={loading} required minlength="8">
  </div>
  <div class="text-center mt-5">      
    <button type="submit" class="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded-full border-none" disabled={loading}>
      Connect
    </button>
  </div>
</form>

