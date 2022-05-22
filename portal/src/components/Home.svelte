<script>
	import {
		onMount
	} from 'svelte';
	import Connect from './Connect.svelte';
	import SelectScan from './SelectScan.svelte';
	import Status from './Status.svelte';

	let data = {
		loading: true,
		connectStatus: {
			sent: false,
			success: true
		},
		selection: {
			direct_connect: false,
			selected: false,
			ssid: '',
      password:'',
      inlist: false,
		},
		access_points: []
	}
  //access_points: [{"name":"Niruhome","password":"abcdef","open":false,"inlist":true},{"name":"Niruhome","password":"","open":false,"inlist":false}]

  let sowdelbut = false;
  let connect_data = {};

	function setConnectSuccess(event){
    sowdelbut = false;
		data.connectStatus.sent = true;
		data.connectStatus.success = true;
		console.log(event);
		connect_data = event.detail.data;
	}

	function setConnectError(event){
    	sowdelbut = false;
		data.connectStatus.sent = true;
		data.connectStatus.success = false;
		console.log(event);
		connect_data = event.detail.data;

	}

	function clearSelection() {
    sowdelbut = false;
		data.selection.selected = false;
		data.selection.direct_connect = false;
		data.selection.ssid = "";
    data.selection.password = "";
    data.selection.inlist = false;
	}

	function selectAccessPoint(event) {
    console.log(event);
    if(event.detail.inlist) {
      sowdelbut = true;
      data.selection.password = event.detail.password;
      data.selection.inlist = true;
    } else {
      data.selection.password = "";
      data.selection.inlist = false;
    }
    
		data.selection.ssid = event.detail.name;
		if(event.detail.open){
			data.selection.direct_connect = true;
		}
		data.selection.selected = true;
	}

	async function refresh (){ 
    sowdelbut = false;
    clearSelection();
		data.loading = true
		await updateAccessPoints();
	}

  async function erase(){
    data.loading = true;
    let formData = new FormData();
    formData.append('ssid', data.selection.ssid);    
    formData.append('optyp', "FEW");
    
    const res = await fetch(`/espnewtune/erase`, { method: 'POST', body: formData });
    
	if (res.status === 200) {      
      await refresh ();
		}else if(res.status === 202) {
			setTimeout(updateAccessPoints, 2000);
		}    
		return res;
  }



	async function updateAccessPoints() {
		const res = await fetch(`/espnewtune/scan`);
		if (res.status === 200) {
			data.access_points = await res.json();
			console.log(data);
			data.loading = false;
		}else if(res.status === 202) {
			setTimeout(updateAccessPoints, 2000);
		}
		return res;
	}

	onMount(async () => {
		try {
			await updateAccessPoints();
		} catch (err) {
			console.log(err);
		}
	});
</script>

<div class="flex flex-col  items-center justify-center w-screen h-screen bg-gray-200 text-gray-700 ">
  
		<div class="flex-col" >
			<!--svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 24 24" class="logo"><path d="M5 12L3 12 3 21 12 21 12 19 5 19zM12 5L19 5 19 12 21 12 21 3 12 3z"></path></svg-->
			<p class = "text-blue-600 text-4xl">⦬ New Tunes ⦬</p>
		</div>	

    
			<div  class="flex flex-col bg-white  rounded shadow-lg p-5 mt-12 mb-12 w-3/4 md:w-1/2 xl:w-1/4 relative pt-10">
        
        <div class="flex flex-row">
          {#if !data.loading}
          <div class="flex group relative">
          <button class=" -mt-10 -ml-5 hover:bg-green-200 text-green-600 font-bold py-2 px-4 rounded-none text-2xl" on:click={refresh}>
            ↻
          </button>   
          <div class="absolute bottom-0 flex flex-col items-center hidden mb-10 group-hover:flex w-20 -ml-8">
            <span class="relative z-10 p-2 text-xs leading-none text-white whitespace-no-wrap bg-black shadow-lg">Refresh List</span>
            <div class="w-3 h-3 -mt-2 rotate-45 bg-black"></div>
          </div>       
        </div>
        {/if}

        <span class="flex flex-grow"></span>

        {#if sowdelbut}
        <div class="flex group relative">
          <button class=" -mt-10 -mr-5 hover:bg-red-200  text-red-500 font-bold py-2 px-4 rounded-none text-2xl " on:click={erase}>
            ✘
            <!--https://www.i2symbol.com/symbols/arrows-->
          </button>    
          <div class="absolute bottom-0 flex flex-col items-center hidden mb-10 group-hover:flex w-20 -ml-3">
            <span class="relative z-10 p-2 text-xs leading-none text-white whitespace-no-wrap bg-black shadow-lg">Delete From List</span>
            <div class="w-3 h-3 -mt-2 rotate-45 bg-black"></div>
          </div>       
        </div>
        {/if}
      </div>

				{#if data.loading}
					<div  class="flex flex-row justify-center p-16  h-100">
            <svg role="status" class="mr-2 w-8 h-8 text-gray-200 animate-spin dark:text-gray-600 fill-blue-600" viewBox="0 0 100 101" fill="none" xmlns="http://www.w3.org/2000/svg">
              <path d="M100 50.5908C100 78.2051 77.6142 100.591 50 100.591C22.3858 100.591 0 78.2051 0 50.5908C0 22.9766 22.3858 0.59082 50 0.59082C77.6142 0.59082 100 22.9766 100 50.5908ZM9.08144 50.5908C9.08144 73.1895 27.4013 91.5094 50 91.5094C72.5987 91.5094 90.9186 73.1895 90.9186 50.5908C90.9186 27.9921 72.5987 9.67226 50 9.67226C27.4013 9.67226 9.08144 27.9921 9.08144 50.5908Z" fill="currentColor"/>
              <path d="M93.9676 39.0409C96.393 38.4038 97.8624 35.9116 97.0079 33.5539C95.2932 28.8227 92.871 24.3692 89.8167 20.348C85.8452 15.1192 80.8826 10.7238 75.2124 7.41289C69.5422 4.10194 63.2754 1.94025 56.7698 1.05124C51.7666 0.367541 46.6976 0.446843 41.7345 1.27873C39.2613 1.69328 37.813 4.19778 38.4501 6.62326C39.0873 9.04874 41.5694 10.4717 44.0505 10.1071C47.8511 9.54855 51.7191 9.52689 55.5402 10.0491C60.8642 10.7766 65.9928 12.5457 70.6331 15.2552C75.2735 17.9648 79.3347 21.5619 82.5849 25.841C84.9175 28.9121 86.7997 32.2913 88.1811 35.8758C89.083 38.2158 91.5421 39.6781 93.9676 39.0409Z" fill="currentFill"/>
            </svg>
					</div>
          
				{:else}
					{#if !data.connectStatus.sent}
						{#if !data.selection.selected}
							<SelectScan access_points={data.access_points} on:refresh={refresh} on:select={selectAccessPoint} />
						{:else}
							<Connect selecteddt={data.selection} on:back={clearSelection} on:success={setConnectSuccess} on:error={setConnectError} />
						{/if}
					{:else}
							<Status success={data.connectStatus.success} {connect_data} />
					{/if}
         
				{/if}
         
			</div>



	
		<div class="text-sm text-muted">
			<p>
				Made with ❤️ by <a href="https://github.com/natrayanp" target="_blank">Natrayan</a>.
			</p>
		<!--
      <p class="text-center">
				<a href="" target="_blank">Buy me a coffee ☕</a>
			</p>
      -->
		</div>
	
  
</div>


